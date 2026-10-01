/* Auteur : Helmi Soudana | Fichier : interface du RTE, ports Sender/Receiver, appels Client/Server et prototypes des runnables */
#ifndef RTE_H
#define RTE_H

#include "../common/Std_Types.h"
#include "../bsw/Dem.h"

#define RTE_E_OK       ((Std_ReturnType)0u)
#define RTE_E_NO_DATA  ((Std_ReturnType)1u)

void Rte_Start(void);
void Rte_Scheduler(void);

void Swc_TempSensor_Run(void);
void Swc_FanController_Run(void);
void Swc_FanActuator_Run(void);
void Swc_Reporter_Run(void);

Std_ReturnType Rte_Write_TempOut_Temp_degC(sint16 value);
Std_ReturnType Rte_Read_TempIn_Temp_degC(sint16* value);

Std_ReturnType Rte_Write_FanOut_FanSpeed_pct(uint8 value);
Std_ReturnType Rte_Read_FanIn_FanSpeed_pct(uint8* value);

Std_ReturnType Rte_Call_IoHw_ReadAdc(uint16* raw);
Std_ReturnType Rte_Call_IoHw_SetPwm(uint8 percent);
Std_ReturnType Rte_Call_Dem_SetEventStatus(Dem_EventIdType id, Dem_EventStatusType st);
Std_ReturnType Rte_Call_Dem_GetEventFailed(Dem_EventIdType id, boolean* failed);
Std_ReturnType Rte_Call_Com_UpdateStatus(sint16 temp, uint8 speed, uint8 flags);

#endif
