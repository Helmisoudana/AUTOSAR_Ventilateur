/* Auteur : Helmi Soudana | Fichier : implementation du Dem, anti-rebond des defauts (5 echecs consecutifs = defaut confirme) */
#include "Dem.h"

#define DEM_FAIL_THRESHOLD  5u

static uint8 debounce[DEM_NB_EVENTS];

void Dem_Init(void)
{
    for (uint8 i = 0; i < DEM_NB_EVENTS; i++) debounce[i] = 0u;
}

Std_ReturnType Dem_SetEventStatus(Dem_EventIdType id, Dem_EventStatusType status)
{
    if (id >= DEM_NB_EVENTS) return E_NOT_OK;

    if (status == DEM_EVENT_STATUS_FAILED) {
        if (debounce[id] < DEM_FAIL_THRESHOLD) debounce[id]++;
    } else {
        if (debounce[id] > 0u) debounce[id]--;
    }
    return E_OK;
}

Std_ReturnType Dem_GetEventFailed(Dem_EventIdType id, boolean* failed)
{
    if (id >= DEM_NB_EVENTS) return E_NOT_OK;
    *failed = (debounce[id] >= DEM_FAIL_THRESHOLD);
    return E_OK;
}
