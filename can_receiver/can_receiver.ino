/* Auteur : Helmi Soudana | Fichier : noeud recepteur CAN (2eme Arduino + MCP2515), decode et affiche la trame 0x100 sur le port serie */
#include <SPI.h>
#include <mcp_can.h>

MCP_CAN CAN0(10);

void setup()
{
    Serial.begin(115200);
    if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
        Serial.println(F("Init CAN echouee"));
        while (1);
    }
    CAN0.setMode(MCP_NORMAL);
    pinMode(2, INPUT);
    Serial.println(F("Moniteur CAN pret"));
}

void loop()
{
    if (!digitalRead(2)) {
        unsigned long id;
        byte len;
        byte buf[8];
        CAN0.readMsgBuf(&id, &len, buf);

        if (id == 0x100 && len == 8) {
            int16_t temp = (int16_t)((buf[0] << 8) | buf[1]);
            Serial.print(F("Temp="));   Serial.print(temp);
            Serial.print(F(" C  Fan=")); Serial.print(buf[2]);
            Serial.print(F("%  Flags=0x")); Serial.print(buf[3], HEX);
            Serial.print(F("  Alive="));  Serial.println(buf[4]);
        }
    }
}
