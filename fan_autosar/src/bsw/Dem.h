/* Auteur : Helmi Soudana | Fichier : interface du Diagnostic Event Manager (Dem), declaration des evenements de defaut */
#ifndef DEM_H
#define DEM_H
#include "../common/Std_Types.h"

typedef uint8 Dem_EventIdType;
#define DEM_EVENT_TEMP_SENSOR  ((Dem_EventIdType)0u)
#define DEM_EVENT_CAN_TX       ((Dem_EventIdType)1u)
#define DEM_NB_EVENTS          2u

typedef enum {
    DEM_EVENT_STATUS_PASSED = 0,
    DEM_EVENT_STATUS_FAILED = 1
} Dem_EventStatusType;

void           Dem_Init(void);
Std_ReturnType Dem_SetEventStatus(Dem_EventIdType id, Dem_EventStatusType status);
Std_ReturnType Dem_GetEventFailed(Dem_EventIdType id, boolean* failed);

#endif
