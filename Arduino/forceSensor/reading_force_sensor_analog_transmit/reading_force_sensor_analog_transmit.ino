#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <Wire.h>
#include <stdlib.h>
#include "can_ext.h"

#define _REF_P_XL 0x08                       //  Reference pressure (LSB)
#define _REF_P_L 0x09                        //  Reference pressure  (middle)
#define _REF_P_H 0x0A                        //  Reference pressure   (MSB)
#define _WHO_AM_I 0x0F                       //  Device identification
#define _RES_CONF 0x10                       //  Pressure resolution
#define _CTRL_REG1 0x20                      //  Control register 1
#define _CTRL_REG2 0x21                      //  Control register 2
#define _CTRL_REG3 0x22                      //  Control register 3
#define _INT_CFG_REG 0x23                    //  Interrupt configuration
#define _INT_SOURCE_REG 0x24                 //  Interrupt source
#define _THS_P_LOW_REG 0x25                  //  Treshold pressure (LSB)
#define _THS_P_HIGH_REG 0x26                 //  Treshold pressure (MSB)
#define _STATUS_REG 0x27                     //  Status register
#define _PRESS_POUT_XL_REH 0x28              //  Pressure data (LSB)
#define _PRESS_OUT_L   0x29                  //  Pressure data (middle)
#define _PRESS_OUT_H   0x2A                  //  Pressure data (MSB)
#define _TEMP_OUT_L 0x2B                     //  Temperature data (LSB)
#define _TEMP_OUT_H 0x2C                     //  Temperature data (MSB)
#define _AMP_CTRL 0x30                       //  Analog front end control
//#define _PRESSURE_W_ADDRESS  0xBA            //  BA       //alt 0xB8
//#define _PRESSURE_R_ADDRESS  0xBB            //  BB       //alt 0xB9
#define _DEVICE_ADDRESS 0X5D                 //  alt 5C     //which doesn't work, hence its definitive that SA0 pad is connected to the voltage supply

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

// ------------------------------------------------------------------------
//  SYSTEM: Setup routine runs on power-up or reset
// ------------------------------------------------------------------------
int error;

int analogPin=3;
int val =0;

void setup(){
  // Set the serial interface baud rate
  Serial.begin(115200);
  
  // Initialize the CAN controller
  if(canInitialize(CAN_250KBPS) == CAN_OK)  // Baud rates defined in mcp_can_dfs.h
    Serial.println("CAN Init OK.\n\r\n\r");
  else
    Serial.println("CAN Init Failed.\n\r");
  }
  
}// end setup

// ------------------------------------------------------------------------
// Main Loop - Arduino Entry Point
// ------------------------------------------------------------------------
void loop(){
  val = analogRead(analogPin);
  byte byte1 = (byte)(val & 0xFF)
  byte byte0 = (byte)( (val>>8) & 0xFF )
  // Declarations
  byte nPriority = 0;
  byte nSrcAddr = 0xff;
  byte nDestAddr = 0xaa;
  byte nData[2] = {byte0, byte1};
  int nDataLen = 2;
  long lPGN = 72;
  byte checker;
  
  checker = j1939Transmit(lPGN, nPriority, nSrcAddr, nDestAddr, nData, nDataLen);
  Serial.println(checker);

  delay(500);
}// end loop
