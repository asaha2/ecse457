// demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    Serial.begin(115200);

START_INIT:

    if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}

unsigned char stmp[8] = {8, 101, 106, 97, 110, 100, 114, 111};
void loop()
{
    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
    
    Serial.println("Sending Data: ");
    
    for(int i=0;i<8;i++){
      Serial.print(stmp[i]);
      Serial.print("\t");
    }
    Serial.println();
    Serial.println("-----------------");
    CAN.sendMsgBuf(0xFF, 0, 8, stmp);
    delay(100);                       // send data per 100ms
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
