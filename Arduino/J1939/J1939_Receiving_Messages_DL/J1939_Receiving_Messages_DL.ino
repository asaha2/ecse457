#include <SD.h>

#include <SPI.h>
#include <stdlib.h>
#include "can_ext.h"
#include <mcp_can.h>
#include <mcp_can_dfs.h>

// ------------------------------------------------------------------------
//  J1939 - Receiving Messages
// ------------------------------------------------------------------------
//
// IMPOPRTANT: Depending on the CAN shield used for this programming sample,
//             please make sure you set the proper CS pin in module can.cpp.
//
//  This Arduino program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.


const int chipSelect = 10;
File dataFile;

// ------------------------------------------------------------------------
//  SYSTEM: Setup routine runs on power-up or reset
// ------------------------------------------------------------------------
void setup()
{
  // Set the serial interface baud rate
  Serial.begin(115200);
  
  // Initialize the CAN controller
  if (canInitialize(CAN_250KBPS) == CAN_OK)  // Baud rates defined in mcp_can_dfs.h
    Serial.print("CAN Init OK.\n\r\n\r");
  else
    Serial.print("CAN Init Failed.\n\r");
  
  // Initialize SD card
  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)){
    Serial.println("Card failed, or not present.");
    // don't do anything more:
    while(1);
  }
  Serial.println("Card initialized.");
  
  // Open up the file we're going to log to
  dataFile = SD.open("datalog.csv", FILE_WRITE);
  if(!dataFile){
    Serial.println("Error opening datalog.txt");
    // Wait forever since we cant write data
    while(1);
  }
}// end setup

// ------------------------------------------------------------------------
// Main Loop - Arduino Entry Point
// ------------------------------------------------------------------------
void loop() 
{
  // Declarations
  byte nPriority;
  byte nSrcAddr;
  byte nDestAddr;
  byte nData[8];
  int nDataLen;
  long lPGN;
  
  char sString[80];
  String dataString = "";
  // Check for received J1939 messages
  if(j1939Receive(&lPGN, &nPriority, &nSrcAddr, &nDestAddr, nData, &nDataLen) == 0)
  {
    sprintf(sString, "PGN: 0x%X Src: 0x%X Dest: 0x%X ", (int)lPGN, nSrcAddr, nDestAddr);
    Serial.print(sString);
    if(nDataLen == 0 )
      Serial.print("No Data.\n\r");
    else
    {
      Serial.print("Data: ");
      for(int nIndex = 0; nIndex < nDataLen; nIndex++)
      {            
        sprintf(sString, "0x%X ", nData[nIndex]);
        Serial.print(sString);
        
      }// end for
      Serial.print("\n\r");
      
      int pressure;
      int byte0 = nData[0];
      int byte1 = nData[1];
      int byte2 = nData[2];
      int byte3 = nData[3];
      pressure = (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | (byte3);
      sprintf(sString,"Pressure: %d",pressure);
      Serial.println(sString);
      dataString = dataString + sString + ", ";
      
      int temperature;
      byte0 = nData[4];
      byte1 = nData[5];
      byte2 = nData[6];
      byte3 = nData[7];
      temperature = (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | (byte3);
      sprintf(sString,"Temperature: %d",temperature);
      Serial.println(sString);
      dataString = dataString + sString;

      if(dataFile.println(dataString) == 0){
        Serial.println("Error writing to file!");
      }
      dataFile.flush();
    }// end else
        
  }// end if

}// end loop


