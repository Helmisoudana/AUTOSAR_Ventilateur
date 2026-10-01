/* Auteur : Helmi Soudana | Fichier : implementation du RTE, relie les SWC entre eux et au BSW, contient l'ordonnanceur periodique */
#include "Rte.h"
#include "../bsw/IoHwAb.h"
#include "../bsw/Dem.h"
#include "../bsw/Com.h"
#include "../bsw/CanIf.h"

static sint16  buf_Temp;
static boolean buf_TempValid = FALSE;
static uint8   buf_FanSpeed;
static boolean buf_FanValid = FALSE;

Std_ReturnType Rte_Write_TempOut_Temp_degC(sint16 value)
{
    buf_Temp = value;
    buf_TempValid = TRUE;
    return RTE_E_OK;
}

Std_ReturnType Rte_Read_TempIn_Temp_degC(sint16* value)
{
    if (!buf_TempValid) return RTE_E_NO_DATA;
    *value = buf_Temp;
    return RTE_E_OK;
}

Std_ReturnType Rte_Write_FanOut_FanSpeed_pct(uint8 value)
{
    buf_FanSpeed = value;
    buf_FanValid = TRUE;
    return RTE_E_OK;
}

Std_ReturnType Rte_Read_FanIn_FanSpeed_pct(uint8* value)
{
    if (!buf_FanValid) return RTE_E_NO_DATA;
    *value = buf_FanSpeed;
    return RTE_E_OK;
}

Std_ReturnType Rte_Call_IoHw_ReadAdc(uint16* raw)  { return IoHwAb_ReadAdc(raw); }
Std_ReturnType Rte_Call_IoHw_SetPwm(uint8 percent) { return IoHwAb_SetPwm(percent); }

Std_ReturnType Rte_Call_Dem_SetEventStatus(Dem_EventIdType id, Dem_EventStatusType st)
{ return Dem_SetEventStatus(id, st); }

Std_ReturnType Rte_Call_Dem_GetEventFailed(Dem_EventIdType id, boolean* failed)
{ return Dem_GetEventFailed(id, failed); }

Std_ReturnType Rte_Call_Com_UpdateStatus(sint16 temp, uint8 speed, uint8 flags)
{ Com_UpdateStatus(temp, speed, flags); return E_OK; }

typedef struct {
    void   (*runnable)(void);
    uint16 period_ms;
    uint32 lastRun;
} Rte_TaskType;

static Rte_TaskType taskTable[] = {
    { Swc_TempSensor_Run,    10u,  0u },
    { Swc_FanController_Run, 20u,  0u },
    { Swc_FanActuator_Run,   20u,  0u },
    { Swc_Reporter_Run,      100u, 0u },
    { Com_MainFunctionTx,    100u, 0u },
};
#define NB_TASKS (sizeof(taskTable) / sizeof(taskTable[0]))

void Rte_Start(void)
{
    IoHwAb_Init();
    Dem_Init();
    Com_Init();

    if (CanIf_Init() != E_OK) {
        Serial.println(F("[ERREUR] Init CAN (MCP2515) echouee"));
    } else {
        Serial.println(F("[OK] CAN pret"));
    }

    uint32 now = millis();
    for (uint8 i = 0; i < NB_TASKS; i++) taskTable[i].lastRun = now;
}

void Rte_Scheduler(void)
{
    uint32 now = millis();
    for (uint8 i = 0; i < NB_TASKS; i++) {
        if ((uint32)(now - taskTable[i].lastRun) >= taskTable[i].period_ms) {
            taskTable[i].lastRun += taskTable[i].period_ms;
            taskTable[i].runnable();
        }
    }
}
