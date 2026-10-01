/* Auteur : Helmi Soudana | Fichier : interface du module COM, empaquetage des signaux dans la trame CAN 0x100 */
#ifndef COM_H
#define COM_H
#include "../common/Std_Types.h"

void Com_Init(void);
void Com_UpdateStatus(sint16 temp_degC, uint8 fanSpeed_pct, uint8 flags);
void Com_MainFunctionTx(void);

#endif
