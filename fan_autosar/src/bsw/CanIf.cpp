/* Auteur : Helmi Soudana | Fichier : implementation de CanIf avec la bibliotheque mcp_can (MCP2515, 500 kbit/s, CS sur D10) */
#include "CanIf.h"
#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS_PIN   10
#define CAN_CRYSTAL  MCP_8MHZ

static MCP_CAN CAN0(CAN_CS_PIN);

Std_ReturnType CanIf_Init(void)
{
    if (CAN0.begin(MCP_ANY, CAN_500KBPS, CAN_CRYSTAL) != CAN_OK) {
        return E_NOT_OK;
    }
#ifdef CAN_LOOPBACK_TEST
    CAN0.setMode(MCP_LOOPBACK);
#else
    CAN0.setMode(MCP_NORMAL);
#endif
    return E_OK;
}

Std_ReturnType CanIf_Transmit(uint32 canId, const uint8* data, uint8 len)
{
    byte status = CAN0.sendMsgBuf(canId, 0, len, (byte*)data);
    return (status == CAN_OK) ? E_OK : E_NOT_OK;
}
