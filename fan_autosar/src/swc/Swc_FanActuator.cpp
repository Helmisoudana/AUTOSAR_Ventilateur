/* Auteur : Helmi Soudana | Fichier : SWC FanActuator (20 ms), applique la vitesse demandee au ventilateur via le PWM */
#include "../rte/Rte.h"

void Swc_FanActuator_Run(void)
{
    uint8 speed;
    if (Rte_Read_FanIn_FanSpeed_pct(&speed) == RTE_E_OK) {
        Rte_Call_IoHw_SetPwm(speed);
    }
}
