/* 
  VOSS - Versatile Open Source Sensor
  
  Jeti EX Telemetry for 
  AttoPilot Voltage and Current Sense Breakout - 45A, 90A, 180A
  https://www.sparkfun.com/products/10643 

  Reads voltage and current from AttoPilot sensor and
  calculates power and used energy.

  Based on the brilliant JetiExSensor library written by Bernd Wokoeck
  
  -------------------------------------------------------------
  Copyright (C) 2016 Wolfgang Korosec
  
  Extended notice on additional work and copyrights, see headers of included files and libraries

  Wiring: 
    Teensy 3.1    TXD-Pin   <-- Select HW UART Serial 1 -3
    Arduino Mini  TXD-Pin 0 <-- Receiver "Ext." input (orange cable)


Arduino Mini Pro 3.3 V
                        |o 10     9 o|
                        |o          o|
                        |o          o|
                        |o          o|
 Attopilot V   <--------|o A0       o|
 Attopilot I   <--------|o A1       o|
                        |o  o A4    o|
                        |o  o A5    o|
                        |o VCC      o|
                        |o          o|
 Attopilot Gnd <--+-----|o GND      o|
                  | +---|o RAW  TXO o|+---+
                  | |                     |
                  | | +-------------------+
                  | | | 
                  v v v
             black red orange
             to Jeti receiver


  Ressources:
    Uses the built-in UART of Arduini Mini Pro 328 or one of 3 UARTs of the Teensy.
    Tested with Teensy 3.1, 3.2 and Arduino Pro Mini 3.3V and AttoPilot 45A
    
  Prerequisites
  Jeti Sensor EX Telemetry C++ Library, Copyright (C) 2015, 2016 Bernd Wokoeck
  https://sourceforge.net/projects/jetiexsensorcpplib/ 

  Version history:
  0.10   2016-07-29  created
  1.0    2016-08-04
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.

**************************************************************/
#define VERSION "VOSS V1.0"
#define SENSORS "AttoPilot 45A"

#define Pin_Volt A0
#define Pin_Amp  A1

#include "JetiExProtocol.h"
#include "AttoPilot_Sensor.h"

JetiExProtocol jetiEx;
Power_Sensor motor;

long volt_ex;
long amp_ex;
long watt_ex;
long capacity_ex;
long temp_ex;

char JetiBoxLine1[40] = {0};
char JetiBoxLine2[40] = {0};
char buffer1[40] = {0};
char buffer2[40] = {0};

enum
{
  ID_CURRENT = 1,
  ID_VOLTAGE,
  ID_WATT,
  ID_CAPACITY,
  ID_TEMP,
};

// id from 1..15
// name plus unit must be < 20 characters
// precision = 0 --> 0, precision = 1 --> 0.0, precision = 2 --> 0.00


JETISENSOR_PTR sensors[] = 
{
  //              id               name         unit         data type             precision 
  new JetiSensor( ID_VOLTAGE,      "Voltage",   "V",         JetiSensor::TYPE_14b, 2 ),
  new JetiSensor( ID_CURRENT,      "Current",   "A",         JetiSensor::TYPE_14b, 2 ), 
  new JetiSensor( ID_WATT,         "Power",     "W",         JetiSensor::TYPE_14b, 0 ),
  new JetiSensor( ID_CAPACITY,     "Capacity",  "mAh",       JetiSensor::TYPE_14b, 0 ),
  new JetiSensor( ID_TEMP,         "Temp",       "\xB0\x43", JetiSensor::TYPE_14b, 1 ),        
  0 // end of array
};


void setup() {  
#ifdef CORE_TEENSY
  Serial.begin( 9600); 
  jetiEx.Start( "VOSS", sensors, JetiExProtocol::SERIAL2); 
#else
  jetiEx.Start( "VOSS", sensors);   
#endif

  motor.Start(Pin_Volt,Pin_Amp,AttoPilot_45A);

  delay(500); 
  sprintf(JetiBoxLine1,"%16s", VERSION);
  sprintf(JetiBoxLine2,"%16s", SENSORS);
      
  jetiEx.SetJetiboxText( JetiExProtocol::LINE1, JetiBoxLine1);
  jetiEx.SetJetiboxText( JetiExProtocol::LINE2, JetiBoxLine2);

  jetiEx.DoJetiSend();

// blink to show that somesthing is going on 
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);   
  delay(2000);               // wait for a second
  digitalWrite(LED, LOW);   
}

void loop() 
{  
  volt_ex = motor.GetVoltage()*100;
  amp_ex = motor.GetCurrent()*100;
  watt_ex = motor.GetPower();
  capacity_ex = motor.GetCapacity();

  jetiEx.SetSensorValue( ID_VOLTAGE, volt_ex);
  jetiEx.SetSensorValue( ID_CURRENT, amp_ex);
  jetiEx.SetSensorValue( ID_WATT,    watt_ex); 
  jetiEx.SetSensorValue( ID_CAPACITY,capacity_ex);
  
  dtostrf(motor.GetVoltage(), 4, 2, buffer1); // sprintf for float not implemented for Arduino so we have to convert!!!
  dtostrf(motor.GetCurrent(), 4, 2, buffer2); // on Teensy sprintf for floats works - very strange ... 
  sprintf(JetiBoxLine1,"%5s V %5s A",buffer1, buffer2);

  dtostrf(motor.GetPower(), 1, 0, buffer1);
  dtostrf(motor.GetCapacity(), 1, 0, buffer2);  
  sprintf(JetiBoxLine2,"%5s W %5smAh",buffer1, buffer2);

  jetiEx.SetJetiboxText( JetiExProtocol::LINE1, JetiBoxLine1);
  jetiEx.SetJetiboxText( JetiExProtocol::LINE2, JetiBoxLine2);

  jetiEx.DoJetiSend();
}
