#include <Wire.h>

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

int error;
long int reading, output;
long int reading2, output2;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  error = Pressure_Init();
  Serial.print("Error value is = ");
  Serial.println(error);
}

void loop() {

  // <--PRESSURE SENSOR READING-->
  reading = (Pressure_Read(_PRESS_OUT_H) << 8) | Pressure_Read(_PRESS_OUT_L);
  reading = (reading << 8) | Pressure_Read(_PRESS_POUT_XL_REH);
  Serial.print(Pressure_Read(_PRESS_OUT_H));
  Serial.print(" & ");
  Serial.print(Pressure_Read(_PRESS_OUT_L));
  Serial.print(" & ");
  Serial.print(Pressure_Read(_PRESS_POUT_XL_REH));
  Serial.print(" | ");

  // if pressure reading value is +ve (msb of MSB is 0)
  // simply divide by the scaling factor 4096
  if((reading >> 23) == 0){
    output = reading / 4096;
    Serial.print("Pressure (mbar) =  ");  
    Serial.print(output);  
    Serial.print(" | ");
//
//    Serial.print("Pressure (psi) = ");
//    Serial.print(output*14.5038);
//    Serial.print(" | ");
  }

  // if pressure reading value is -ve (msb of MSB is 1)
  // take 2's complement (invert and add 1)
  // and then divide by the scaling factor
  else{
    output = (reading ^ 1) + 1;
    output = output / 4096;
    Serial.print("Pressure (mbar) = -");
    Serial.print(output);  
    Serial.print(" | "); 
  }

  // <--TEMPERATURE SENSOR READING-->
  reading2 = (Pressure_Read(_TEMP_OUT_H) << 8) | Pressure_Read(_TEMP_OUT_L); 

  // if the temperature reading value is +ve
  // simply divide by scaling factor and then add offset
  if((reading2 >> 15) == 0){
    output2 = (reading2 / 480) + 42.5;
    Serial.print("Temperature (degree centigrade) = ");
    Serial.println(output2);
  }

  // if temperature reading is -ve, take 2's complement
  // and divide by scaling factor and add offset to it
  else{
    output2 = (reading2 ^ 1) + 1;
    output2 = (output2 / 480) + 42.5;
    Serial.print("Temperature (degree centigrade) = ");
    Serial.println(output2);
  }
   
  delay(500);
}

/*  Declaration of function, which will initiate I2C communication on relation Miccrocontroler - Pressure Click
    and she will write one byte through I2C, onto memory address on the Pressure Click */
void Pressure_Write(int address, int value) {                         
  Wire.beginTransmission(_DEVICE_ADDRESS);      
  Wire.write(address);
  Wire.write(value);                    
  Wire.endTransmission();     
}

//  Declaration of function, which will read the measured data from Pressure Click, through I2C communication.
int Pressure_Read(int address)  {
  int tmp;
  
  Wire.beginTransmission(_DEVICE_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
    
  Wire.requestFrom(_DEVICE_ADDRESS, 1, true);
  while(Wire.available()){
    tmp = Wire.read();  
//    Serial.println(tmp);  
  }

//  <--Debugger code-->
//  if(Wire.available() > 0){
//    while(Wire.available()){
//      tmp = Wire.read();  
//      Serial.println(tmp);  
//    }
//  }
//  else{
//    Serial.println("No data");
//  }
  
  return tmp;
}

//  Declaration of function, which will initiate Pressure values
int Pressure_Init(void)  {
  int temp;                                  // Declaration of local variable
  int err = 0;                               // Declaration of local variable

  Pressure_Write(_RES_CONF, 0x78);           // Initiating the pressure resolution on Pressure Click with writing HEX value in his register ( in this case HEX 0x78)
  temp = Pressure_Read(_RES_CONF);           // Begining of checking of entered value in the register _RES_CONF of the Pressure Click
  Serial.println(temp);
  if (temp != 0x78) {
    err++;
  }                                          // End of checking of entered value in the register _RES_CONF of the Pressure Click

  Pressure_Write(_CTRL_REG1, 0x74);
  temp = Pressure_Read(_CTRL_REG1);          // Begining of checking of entered value in the register _CTRL_REG1 (Control register 1) of the Pressure Click
  Serial.println(temp);
  if (temp != 0x74) {
    err++;
  }                                          // End of checking of entered value in the register _CTRL_REG1 (Control register 1) of the Pressure Click

  Pressure_Write(_CTRL_REG1, 0xF4);
  temp = Pressure_Read(_CTRL_REG1);          // Begining of checking of entered value in the register _CTRL_REG1 (Control register 1) of the Pressure Click
  Serial.println(temp);
  if (temp != 0xF4) {
    err++;
  }                                          // End of checking of entered value in the register _CTRL_REG1 (Control register 1) of the Pressure Click

  temp = Pressure_Read(_WHO_AM_I);           // Checking the read-only register from Pressure Click, which contains the device ID (Adresse of Device). For LPS331AP, is set to BBh
  Serial.println(temp);
  if (temp != 0xBB) {
    err++;
  }

  return err;
}


