/* Auteur : Helmi Soudana | Fichier : implementation Arduino de IoHwAb, seule couche qui connait les broches A0 (NTC) et D9 (ventilateur) */
#include "IoHwAb.h"

#define PIN_NTC  A0
#define PIN_FAN  9

void IoHwAb_Init(void)
{
    pinMode(PIN_FAN, OUTPUT);
    analogWrite(PIN_FAN, 0);
}

Std_ReturnType IoHwAb_ReadAdc(uint16* raw)
{
    *raw = (uint16)analogRead(PIN_NTC);
    return E_OK;
}

Std_ReturnType IoHwAb_SetPwm(uint8 percent)
{
    if (percent > 100u) percent = 100u;
    analogWrite(PIN_FAN, (int)(((uint16)percent * 255u) / 100u));
    return E_OK;
}
