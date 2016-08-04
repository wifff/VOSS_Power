#ifndef POWER_SENSOR_H
#define POWER_SENSOR_H

#define AttoPilot_45A  1
#define AttoPilot_90A  2
#define AttoPilot_180A 3
#define LED 13

class Power_Sensor 
{       
  public:   
    Power_Sensor();
    float GetVoltage(); 
    float GetCurrent();
    float GetPower();
    float GetCapacity();                    

    void Start(int _pin_voltage, int _pin_current, int _amp_board);
    
  protected:
    int pin_voltage;
    int pin_current;
    int amp_board;   
    int amp_raw;
    int volt_raw;
    float factor_amp;
    float factor_volt;
    float volt;
    float amp;
    float watt;
    float capacity;     
    unsigned long timer_volt;
    unsigned long timer_amp;
    unsigned long timer_watt;
    unsigned long timer_mAh;        
    int read_delay;
    boolean led_on;
}; 

#endif
