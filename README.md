# Mini-projet AUTOSAR : régulation de ventilateur (Arduino + MCP2515)

**Auteur : Helmi Soudana**

## 1. Présentation

Système de refroidissement : une thermistance NTC mesure la température, un contrôleur décide de la vitesse du ventilateur, un actionneur applique le PWM, et l'état complet est publié sur un bus CAN (trame `0x100`).

> Ce projet est une **mini-implémentation inspirée d'AUTOSAR Classic**, à but pédagogique. Ce n'est pas une pile AUTOSAR certifiée : un vrai AUTOSAR vise des microcontrôleurs 32 bits avec des outils sous licence. Ici on reprend l'architecture, les noms et les principes (SWC, RTE, BSW, Dem, Com, CanIf, IoHwAb).

## 2. Intérêt d'AUTOSAR dans ce projet

| Problème sans AUTOSAR | Apport de l'approche AUTOSAR |
|---|---|
| Logique mélangée avec `analogRead()`, `analogWrite()`, SPI | Les SWC ne touchent jamais le matériel, ils passent par le RTE |
| Changer de carte = tout réécrire | Seuls `IoHwAb` et `CanIf` changent, les SWC restent identiques |
| Boucles et `delay()` partout | Un ordonnanceur central appelle les runnables à périodes fixes |
| Gestion d'erreurs aléatoire | Le Dem centralise les défauts avec anti-rebond |
| Octets CAN écrits à la main | Le module Com empaquette les signaux à un seul endroit |
| Tests impossibles sans matériel | Les SWC se testent sur PC avec un faux RTE |

## 3. Arborescence

```
Projet_AUTOSAR_Ventilateur/
├── README.md
├── fan_autosar/                  Sketch Arduino principal (nœud émetteur)
│   ├── fan_autosar.ino           Point d'entrée : Rte_Start() puis Rte_Scheduler()
│   └── src/
│       ├── common/
│       │   └── Std_Types.h       Types standard AUTOSAR
│       ├── bsw/                  Basic Software
│       │   ├── Dem.h / Dem.cpp           Diagnostic des défauts
│       │   ├── IoHwAb.h / IoHwAb.cpp     Abstraction matérielle (ADC, PWM)
│       │   ├── CanIf.h / CanIf.cpp       Interface CAN (MCP2515)
│       │   └── Com.h / Com.cpp           Empaquetage de la trame 0x100
│       ├── rte/
│       │   └── Rte.h / Rte.cpp           Runtime Environment + ordonnanceur
│       └── swc/                  Composants applicatifs
│           ├── Swc_TempSensor.cpp
│           ├── Swc_FanController.cpp
│           ├── Swc_FanActuator.cpp
│           └── Swc_Reporter.cpp
└── can_receiver/
    └── can_receiver.ino          Sketch du 2ème Arduino (récepteur CAN)
```

Chaque fichier de code commence par un commentaire unique indiquant l'auteur et le rôle du fichier.

## 4. Architecture en couches

```
┌───────────────────────── Couche Application ─────────────────────────┐
│ Swc_TempSensor → Swc_FanController → Swc_FanActuator    Swc_Reporter │
└───────────────────────────────┬──────────────────────────────────────┘
                     ┌──────────┴──────────┐
                     │   RTE (Rte.cpp)     │  ports S/R + appels C/S + ordonnanceur
                     └──────────┬──────────┘
┌───────────────────────────────┴──────────────────────────────────────┐
│ BSW : IoHwAb (ADC/PWM) │ Dem (diagnostic) │ Com → CanIf → MCP2515    │
└───────────────────────────────┬──────────────────────────────────────┘
                        Matériel Arduino (ADC, PWM, SPI)
```

Règle fondamentale : **les SWC n'incluent que `Rte.h`**. Ils ne connaissent ni le matériel, ni les autres SWC.

## 5. Liaisons entre les fichiers

### 5.1 Dépendances (qui inclut qui)

| Fichier | Dépend de |
|---|---|
| `fan_autosar.ino` | `rte/Rte.h` |
| `Rte.h` | `Std_Types.h`, `Dem.h` |
| `Rte.cpp` | `Rte.h`, `IoHwAb.h`, `Dem.h`, `Com.h`, `CanIf.h` |
| `Swc_*.cpp` | `Rte.h` uniquement |
| `Com.cpp` | `Com.h`, `CanIf.h`, `Dem.h` |
| `CanIf.cpp` | `CanIf.h`, bibliothèque `mcp_can` |
| `IoHwAb.cpp` | `IoHwAb.h` |
| `Dem.cpp` | `Dem.h` |

### 5.2 Ports Sender/Receiver (données entre SWC, via le RTE)

| Interface | Élément | Écrivain | Lecteurs |
|---|---|---|---|
| `If_Temperature` | `Temp_degC` (sint16) | `Swc_TempSensor` | `Swc_FanController`, `Swc_Reporter` |
| `If_FanCommand` | `FanSpeed_pct` (uint8, 0..100) | `Swc_FanController` | `Swc_FanActuator`, `Swc_Reporter` |

### 5.3 Appels Client/Server (SWC vers BSW, via le RTE)

| Appel RTE | Redirigé vers | Utilisé par |
|---|---|---|
| `Rte_Call_IoHw_ReadAdc` | `IoHwAb_ReadAdc` | TempSensor |
| `Rte_Call_IoHw_SetPwm` | `IoHwAb_SetPwm` | FanActuator |
| `Rte_Call_Dem_SetEventStatus` | `Dem_SetEventStatus` | TempSensor |
| `Rte_Call_Dem_GetEventFailed` | `Dem_GetEventFailed` | FanController, Reporter |
| `Rte_Call_Com_UpdateStatus` | `Com_UpdateStatus` | Reporter |

### 5.4 Flux complet d'une mesure

```
NTC → A0 → IoHwAb_ReadAdc → [RTE] → Swc_TempSensor (validation, filtre, °C)
   → Rte_Write Temp → Swc_FanController (hystérésis, vitesse)
   → Rte_Write FanSpeed → Swc_FanActuator → IoHwAb_SetPwm → D9 → MOSFET → ventilateur

Swc_Reporter → Com_UpdateStatus → Com_MainFunctionTx → CanIf_Transmit → MCP2515 → bus CAN
```

## 6. Ordonnancement (table de tâches dans `Rte.cpp`)

| Runnable | Période | Rôle |
|---|---|---|
| `Swc_TempSensor_Run` | 10 ms | Lecture, validation, filtrage, conversion en °C |
| `Swc_FanController_Run` | 20 ms | Hystérésis, vitesse proportionnelle, mode dégradé |
| `Swc_FanActuator_Run` | 20 ms | Application du PWM |
| `Swc_Reporter_Run` | 100 ms | État pour le CAN et logs série (toutes les 500 ms) |
| `Com_MainFunctionTx` | 100 ms | Émission de la trame CAN `0x100` |

## 7. Logique de contrôle

- Le ventilateur démarre au-dessus de **35 °C** et s'arrête sous **30 °C** (hystérésis).
- Vitesse proportionnelle de 30 % à 100 % entre 30 °C et 50 °C.
- **Mode dégradé** : si le capteur est en défaut confirmé, le ventilateur passe à 100 %.
- **Dem** : un défaut est confirmé après 5 échecs consécutifs et disparaît après 5 succès consécutifs.
- Validation ADC : valeur < 10 (court-circuit) ou > 1013 (capteur débranché) = échec.

## 8. Trame CAN `0x100` (500 kbit/s, 8 octets)

| Octet | Contenu |
|---|---|
| 0-1 | Température en °C (sint16, big-endian) |
| 2 | Vitesse ventilateur en % |
| 3 | Drapeaux : bit0 = ventilateur ON, bit1 = défaut capteur |
| 4 | Alive counter (incrémenté à chaque envoi) |
| 5-7 | Réservés (0) |

## 9. Matériel et câblage

Composants : Arduino Uno/Nano, module MCP2515 (TJA1050), thermistance NTC 10 kΩ (B = 3950), résistance 10 kΩ, MOSFET logique (IRLZ44N), résistance 220 Ω (gate), résistance 10 kΩ (gate vers GND), diode de roue libre sur le ventilateur, alimentation externe pour le ventilateur.

| MCP2515 | Arduino |
|---|---|
| VCC | 5V |
| GND | GND |
| CS | D10 |
| SI (MOSI) | D11 |
| SO (MISO) | D12 |
| SCK | D13 |
| INT | D2 (utilisé par le récepteur) |

- **Thermistance** : 5V → 10 kΩ → **A0** → NTC → GND
- **Ventilateur** : D9 → 220 Ω → gate du MOSFET ; drain vers le ventilateur ; source vers GND ; masses communes.
- **Bus CAN** : CANH-CANH, CANL-CANL, terminaison 120 Ω aux deux extrémités.

## 10. Installation

1. Installer la bibliothèque **mcp_can** (coryjfowler) via le gestionnaire de bibliothèques Arduino.
2. Ouvrir `fan_autosar/fan_autosar.ino` (le dossier `src/` est compilé automatiquement), téléverser sur l'Arduino principal.
3. Ouvrir le moniteur série à **115200 bauds**.
4. Pour le récepteur : ouvrir `can_receiver/can_receiver.ino` et téléverser sur le 2ème Arduino.

Réglages dans `src/bsw/CanIf.cpp` :
- Quartz du module : `MCP_8MHZ` ou `MCP_16MHZ` (dans les deux sketchs).
- Test avec **un seul module** (sans autre nœud, pas d'ACK) : ajouter `#define CAN_LOOPBACK_TEST` en haut de `CanIf.cpp`.

## 11. Tests

1. Au démarrage : `[OK] CAN pret` puis des lignes `T=24 C | Fan=0% | ...`.
2. Chauffer la thermistance : au-dessus de 35 °C le ventilateur démarre et accélère jusqu'à 100 % vers 50 °C.
3. Débrancher la thermistance : après environ 50 ms le défaut est confirmé, le ventilateur passe à 100 % et le bit1 des drapeaux passe à 1.
4. Récepteur : une trame toutes les 100 ms avec l'alive counter qui s'incrémente.

## 12. Dépannage

| Symptôme | Piste |
|---|---|
| `Init CAN echouee` | Vérifier le câblage SPI et le quartz (`MCP_16MHZ`) |
| Aucune trame reçue | Terminaison 120 Ω manquante, vitesses différentes, ou un seul nœud sur le bus |
| Température absurde | Vérifier le diviseur (NTC côté GND) et la valeur de B |
| Sifflement du ventilateur | PWM de D9 à ~490 Hz, normal |

## 13. Pour aller plus loin

- Écrire les fichiers **ARXML** (SWC, ports, interfaces).
- Ajouter la réception CAN (trame `0x200` pour forcer le ventilateur) dans `Com`.
- Stocker les défauts en EEPROM (rôle de la NvM).
- Porter sur STM32 en ne réécrivant que `IoHwAb` et `CanIf`.
- Tester les SWC sur PC avec un faux RTE.
