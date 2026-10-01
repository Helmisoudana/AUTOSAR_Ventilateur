/* Auteur : Helmi Soudana | Fichier : interface CAN (CanIf), isole le controleur MCP2515 du reste du logiciel */
#ifndef CANIF_H
#define CANIF_H
#include "../common/Std_Types.h"

Std_ReturnType CanIf_Init(void);
Std_ReturnType CanIf_Transmit(uint32 canId, const uint8* data, uint8 len);

#endif
