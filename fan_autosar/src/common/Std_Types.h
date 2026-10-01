/* Auteur : Helmi Soudana | Fichier : types standard AUTOSAR (uint8, sint16, Std_ReturnType, E_OK, E_NOT_OK) */
#ifndef STD_TYPES_H
#define STD_TYPES_H

#include <Arduino.h>

typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef int8_t    sint8;
typedef int16_t   sint16;

typedef uint8_t Std_ReturnType;
#define E_OK      ((Std_ReturnType)0u)
#define E_NOT_OK  ((Std_ReturnType)1u)

#ifndef TRUE
#define TRUE  true
#define FALSE false
#endif

#endif
