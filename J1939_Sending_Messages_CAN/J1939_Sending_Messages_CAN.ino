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
long int reading, output;
int reading2, output2;

void setup(){
  // Set the serial interface baud rate
  Serial.begin(115200);
  
  // Initialize the CAN controller
  if(canInitialize(CAN_250KBPS) == CAN_OK)  // Baud rates defined in mcp_can_dfs.h
    Serial.println("CAN Init OK.\n\r\n\r");
  else
    Serial.println("CAN Init Failed.\n\r");
    
  Wire.begin();
  error = Pressure_Init();
  if(error == 0){
    Serial.println("Sensor Init OK.");
  }
  else{
    Serial.print("Sensor Init Failed. Error = ");
    Serial.println(error);
  }    
}// end setup

// ------------------------------------------------------------------------
// Main Loop - Arduino Entry Point
// ------------------------------------------------------------------------
void loop(){

  // <--PRESSURE SENSOR READING-->
  reading = (Pressure_Read(_PRESS_OUT_H) << 8) | Pressure_Read(_PRESS_OUT_L);
  reading = (reading << 8) | Pressure_Read(_PRESS_POUT_XL_REH);

  // if pressure reading value is +ve (msb of MSB is 0) simply divide by the scaling factor 4096
  if((reading >> 23) == 0){
    output = reading / 4096;        // Readings in mbar
  }
  // if pressure reading value is -ve (msb of MSB is 1) take 2's complement (invert and add 1) and then divide by the scaling factor
  else{
    output = (reading ^ 1) + 1;
    output = output / 4096;         // Readings in mbar
  }

  // <--TEMPERATURE SENSOR READING-->
  reading2 = (Pressure_Read(_TEMP_OUT_H) << 8) | Pressure_Read(_TEMP_OUT_L); 
  
  // if the temperature reading value is +ve simply divide by scaling factor and then add offset
  if((reading2 >> 15) == 0){
    output2 = (reading2 / 480) + 42.5;  // Readings in degree centigrade
  }  
  // if temperature reading is -ve, take 2's complement and divide by scaling factor and add offset to it
  else{
    output2 = (reading2 ^ 1) + 1;
    output2 = (output2 / 480) + 42.5; // Readings in degree centigrade
  }
//
//  Serial.print(reading);
//  Serial.print(" | ");
  Serial.println(output);
//  Serial.print(" | ");
//  Serial.print(reading2);
//  Serial.print(" | ");
  Serial.println(output2);

  int mask = 0xFF;
  byte press1 = (output >> 24) & mask;
  byte press2 = (output >> 16) & mask;
  byte press3 = (output >> 8) & mask;
  byte press4 = output & mask;
  
  byte temp1 = (output2 >> 24) & mask;
  byte temp2 = (output2 >> 16) & mask;
  byte temp3 = (output2 >> 8) & mask;
  byte temp4 = output2 & mask; 
  
  // Declarations
  byte nPriority = 0;
  byte nSrcAddr = 0xff;
  byte nDestAddr = 0xaa;
  byte nData[8] = {press1, press2, press3, press4, temp1, temp2, temp3, temp4};
  int nDataLen = 8;
  long lPGN = 72;
  byte checker;
  
  checker = j1939Transmit(lPGN, nPriority, nSrcAddr, nDestAddr, nData, nDataLen);
  Serial.println(checker);

  delay(500);
}// end loop

// Declaration of function, which will initiate Pressure values
int Pressure_Init(void){
  int temp;                                  // Declaration of local variable
  int err = 0;                               // Declaration of local variable

  Pressure_Write(_RES_CONF, 0x78);           // Initiating the pressure resolution on Pressure Click with writing HEX value in his register ( in this case HEX 0x78)
  temp = Pressure_Read(_RES_CONF);           // Begining of checking of entered value in the register _RES_CONF of the Pressure Click
  Serial.println(temp);
  if(temp != 0x78){
    err++;
  }                                          // End of checking of entered value in the register _RES_CONF of the Pressure Click

  Pressure_Write(_CTRL_REG1, 0x74);
  temp = Pressure_Read(_CTRL_REG1);          // Begining of checking of entered value in the register _CTRL_REG1 (Control register 1) of the Pressure Click
  Serial.println(temp);
  if(temp != 0x74){
    err++;
  }                                          // End of checking of entered value in the register _CTRL_REG1 (Control register 1) of the Pressure Click

  Pressure_Write(_CTRL_REG1, 0xF4);
  temp = Pressure_Read(_CTRL_REG1);          // Begining of checking of entered value in the register _CTRL_REG1 (Control register 1) of the Pressure Click
  Serial.println(temp);
  if(temp != 0xF4){
    err++;
  }                                          // End of checking of entered value in the register _CTRL_REG1 (Control register 1) of the Pressure Click

  temp = Pressure_Read(_WHO_AM_I);           // Checking the read-only register from Pressure Click, which contains the device ID (Adresse of Device). For LPS331AP, is set to BBh
  Serial.println(temp);
  if(temp != 0xBB){
    err++;
  }

  return err;
}

// Declaration of function, which will initiate I2C communication on relation Miccrocontroler - Pressure Click
// and she will write one byte through I2C, onto memory address on the Pressure Click
void Pressure_Write(int address, int value){                         
  Wire.beginTransmission(_DEVICE_ADDRESS);      
  Wire.write(address);
  Wire.write(value);                    
  Wire.endTransmission();     
}

// Declaration of function, which will read the measured data from Pressure Click, through I2C communication.
int Pressure_Read(int address)  {
  int tmp;
  
  Wire.beginTransmission(_DEVICE_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
    
  Wire.requestFrom(_DEVICE_ADDRESS, 1, true);
  while(Wire.available()){
    tmp = Wire.read();  
  }
  return tmp;
}

