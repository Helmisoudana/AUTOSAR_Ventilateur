/* Auteur : Helmi Soudana | Fichier : SWC TempSensor (10 ms), lit l'ADC, valide, filtre et convertit la mesure NTC en degres Celsius */
#include "../rte/Rte.h"
#include <math.h>

#define NTC_R_SERIES   10000.0f
#define NTC_R_NOMINAL  10000.0f
#define NTC_B_COEFF    3950.0f
#define NTC_T_NOMINAL  298.15f

#define ADC_MIN_VALID  10u
#define ADC_MAX_VALID  1013u

static uint16  filteredRaw = 0u;
static boolean firstSample = TRUE;

void Swc_TempSensor_Run(void)
{
    uint16 raw;

    if (Rte_Call_IoHw_ReadAdc(&raw) != E_OK) {
        Rte_Call_Dem_SetEventStatus(DEM_EVENT_TEMP_SENSOR, DEM_EVENT_STATUS_FAILED);
        return;
    }

    if (raw < ADC_MIN_VALID || raw > ADC_MAX_VALID) {
        Rte_Call_Dem_SetEventStatus(DEM_EVENT_TEMP_SENSOR, DEM_EVENT_STATUS_FAILED);
        return;
    }
    Rte_Call_Dem_SetEventStatus(DEM_EVENT_TEMP_SENSOR, DEM_EVENT_STATUS_PASSED);

    if (firstSample) { filteredRaw = raw; firstSample = FALSE; }
    else             { filteredRaw = (uint16)((filteredRaw * 3u + raw) / 4u); }

    float rNtc = NTC_R_SERIES * (float)filteredRaw / (1023.0f - (float)filteredRaw);
    float tK   = 1.0f / ((1.0f / NTC_T_NOMINAL) + (logf(rNtc / NTC_R_NOMINAL) / NTC_B_COEFF));
    float tC   = tK - 273.15f;

    Rte_Write_TempOut_Temp_degC((sint16)lroundf(tC));
}
