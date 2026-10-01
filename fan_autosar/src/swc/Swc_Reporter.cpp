/* Auteur : Helmi Soudana | Fichier : SWC Reporter (100 ms), prepare l'etat envoye sur le CAN et affiche les logs sur le port serie */
#include "../rte/Rte.h"

void Swc_Reporter_Run(void)
{
    static uint8 printDivider = 0u;

    sint16  temp = 0;
    uint8   speed = 0u;
    boolean sensorFailed = FALSE;

    Rte_Read_TempIn_Temp_degC(&temp);
    Rte_Read_FanIn_FanSpeed_pct(&speed);
    Rte_Call_Dem_GetEventFailed(DEM_EVENT_TEMP_SENSOR, &sensorFailed);

    uint8 flags = 0u;
    if (speed > 0u)   flags |= 0x01u;
    if (sensorFailed) flags |= 0x02u;

    Rte_Call_Com_UpdateStatus(temp, speed, flags);

    if (++printDivider >= 5u) {
        printDivider = 0u;
        Serial.print(F("T="));      Serial.print(temp);
        Serial.print(F(" C | Fan=")); Serial.print(speed);
        Serial.print(F("% | Defaut capteur=")); Serial.println(sensorFailed ? F("OUI") : F("non"));
    }
}
