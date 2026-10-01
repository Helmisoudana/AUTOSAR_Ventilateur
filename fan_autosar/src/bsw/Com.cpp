/* Auteur : Helmi Soudana | Fichier : implementation de COM, construit et emet la trame 0x100 (temperature, vitesse, drapeaux, alive counter) toutes les 100 ms */
#include "Com.h"
#include "CanIf.h"
#include "Dem.h"

#define COM_PDU_STATUS_ID  0x100u

static sint16 sigTemp;
static uint8  sigSpeed;
static uint8  sigFlags;
static uint8  aliveCounter;

void Com_Init(void)
{
    sigTemp = 0; sigSpeed = 0; sigFlags = 0; aliveCounter = 0;
}

void Com_UpdateStatus(sint16 temp_degC, uint8 fanSpeed_pct, uint8 flags)
{
    sigTemp  = temp_degC;
    sigSpeed = fanSpeed_pct;
    sigFlags = flags;
}

void Com_MainFunctionTx(void)
{
    uint8 pdu[8];
    uint16 t = (uint16)sigTemp;

    pdu[0] = (uint8)(t >> 8);
    pdu[1] = (uint8)(t & 0xFFu);
    pdu[2] = sigSpeed;
    pdu[3] = sigFlags;
    pdu[4] = aliveCounter++;
    pdu[5] = 0u; pdu[6] = 0u; pdu[7] = 0u;

    if (CanIf_Transmit(COM_PDU_STATUS_ID, pdu, 8u) == E_OK) {
        Dem_SetEventStatus(DEM_EVENT_CAN_TX, DEM_EVENT_STATUS_PASSED);
    } else {
        Dem_SetEventStatus(DEM_EVENT_CAN_TX, DEM_EVENT_STATUS_FAILED);
    }
}
