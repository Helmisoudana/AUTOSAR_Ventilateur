/* Auteur : Helmi Soudana | Fichier : SWC FanController (20 ms), hysteresis ON/OFF, vitesse proportionnelle et mode degrade si capteur en defaut */
#include "../rte/Rte.h"

#define TEMP_FAN_ON    35
#define TEMP_FAN_OFF   30
#define TEMP_FAN_FULL  50
#define SPEED_MIN_ON   30

static boolean fanOn = FALSE;

void Swc_FanController_Run(void)
{
    sint16  temp;
    uint8   speed = 0u;
    boolean sensorFailed = FALSE;

    Rte_Call_Dem_GetEventFailed(DEM_EVENT_TEMP_SENSOR, &sensorFailed);

    if (sensorFailed) {
        Rte_Write_FanOut_FanSpeed_pct(100u);
        return;
    }

    if (Rte_Read_TempIn_Temp_degC(&temp) != RTE_E_OK) {
        Rte_Write_FanOut_FanSpeed_pct(0u);
        return;
    }

    if (temp > TEMP_FAN_ON)       fanOn = TRUE;
    else if (temp < TEMP_FAN_OFF) fanOn = FALSE;

    if (fanOn) {
        sint16 s = SPEED_MIN_ON +
                   (sint16)(((int32_t)(temp - TEMP_FAN_OFF) * (100 - SPEED_MIN_ON))
                            / (TEMP_FAN_FULL - TEMP_FAN_OFF));
        if (s > 100) s = 100;
        if (s < 0)   s = 0;
        speed = (uint8)s;
    }

    Rte_Write_FanOut_FanSpeed_pct(speed);
}
