/* Auteur : Helmi Soudana | Fichier : interface de la couche d'abstraction materielle IO (lecture ADC, commande PWM) */
#ifndef IOHWAB_H
#define IOHWAB_H
#include "../common/Std_Types.h"

void           IoHwAb_Init(void);
Std_ReturnType IoHwAb_ReadAdc(uint16* raw);
Std_ReturnType IoHwAb_SetPwm(uint8 percent);

#endif
