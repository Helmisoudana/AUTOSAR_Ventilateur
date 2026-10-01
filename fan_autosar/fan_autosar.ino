/* Auteur : Helmi Soudana | Fichier : point d'entree Arduino, demarre le RTE puis execute l'ordonnanceur en boucle */
#include "src/rte/Rte.h"

void setup()
{
    Serial.begin(115200);
    Rte_Start();
}

void loop()
{
    Rte_Scheduler();
}
