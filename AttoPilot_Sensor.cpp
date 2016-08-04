#include "Arduino.h"
#include "AttoPilot_Sensor.h"
/* 
AttoPilot Voltage and Current Sense Breakout - 45A, 90A, 180A
https://www.sparkfun.com/products/10643 

Analog Voltage Outputs for Sensed Parameters:
o 50V/180A = 63.69mV / Volt 18.30mV / Amp
o 50V/90A = 63.69mV / Volt 36.60mV / Amp
o 13.6V/45A = 242.3mV / Volt 73.20mV / Amp

// Analog Pin: 1024 = 3.3V (for 3.3V boards!) => 310.303 Steps/Volt 
13.6V/45A:
Volt real = steps * (1/310.303)/0.2423 = 0.01330027472022
Amp real =  steps * (1/310.303)/0.0732 = 0.04384566754707

50V/90A:
Volt real = steps * (1/310.303)/0.06369 = 0.00020525099661
Amp real =  steps * (1/310.303)/0.03660 = 0.08805072581175

50V/180A:
Volt real = steps * (1/310.303)/0.06369 = 0.00020525099661
Amp real =  steps * (1/310.303)/0.01830 = 0.1761014516235
*/

Power_Sensor::Power_Sensor() {}

float Power_Sensor::GetVoltage()
{
  if( ( timer_volt + read_delay ) <= millis() )
  {
    volt_raw = analogRead(pin_voltage);    
    volt = (float) volt_raw * factor_volt;  // Analog Pin: 1024 = 3.3V (for 3.3V boards!) => 310.303 Steps/Volt 
    timer_volt = millis();     
     
    if (led_on)  digitalWrite(LED, HIGH); else digitalWrite(LED, LOW); // blink = active sensor
    led_on = ! led_on; 
  }
  return volt;
}


float Power_Sensor::GetCurrent()
{
  if( ( timer_amp + read_delay ) <= millis() )
  {
    amp_raw = analogRead(pin_current);
    amp = (float) amp_raw * factor_amp;
    timer_amp = millis();     
  }
  return amp;
}


float Power_Sensor::GetPower()
{
  if( ( timer_watt + read_delay ) <= millis() )
  {
    volt_raw = analogRead(pin_voltage);
    amp_raw = analogRead(pin_current);
    watt = (float) volt_raw * factor_volt * (float) amp_raw * factor_amp;    
    timer_watt = millis();     
  }
  return watt;
}

float Power_Sensor::GetCapacity()
{
  if((timer_mAh + read_delay ) <= millis() )
  {   
    amp_raw = analogRead(pin_current);
    capacity = capacity + (amp_raw * factor_amp) * (millis() - timer_mAh) / 3600.0;     
    timer_mAh = millis();     
  }
  return capacity;
}

void Power_Sensor::Start(int _pin_voltage, int _pin_current, int _amp_board )
{
  pin_voltage = _pin_voltage;
  pin_current = _pin_current;
  volt = 0;
  amp = 0;
  read_delay = 200;
  led_on = true;

  switch (_amp_board) 
  {
    case 1:
      factor_volt = 0.013320; // correct value if necessary
      factor_amp =  0.05100;  // used Unisense-E for calibration
      break;
    case 2:
      factor_volt = 0.000205;
      factor_amp =  0.088050;
      break;
    case 3:
      factor_volt = 0.000205;
      factor_amp =  0.176101;
      break;     
  } 
}
