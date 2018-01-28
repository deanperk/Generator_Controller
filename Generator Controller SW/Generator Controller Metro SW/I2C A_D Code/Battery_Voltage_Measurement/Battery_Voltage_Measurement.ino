/*
 * Battery Voltage Measurement
 */
#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1015 ads1015;
/* ADC Scaling]
 *  12 bits or 4096 counts
 *  Single ended useage is 2048 counts
 *  Full scale is 6.144V or 3mV per bit
 *  5V measurement is 5/6.144 * 2048 = 1667bits
 *  1667 bits X 0.003 Volts per bit = 5V 
 */
// declare variables
   float battVolt=0.0;

//Define system battery voltage read function
  float readBatt() {
  float battVolt=0.0;
  unsigned int readVoltage = 0; 
   for (int i=0; i<5; i++) {
   readVoltage = readVoltage + ads1015.readADC_SingleEnded(3);
   delay(10);
   }  
battVolt = ((readVoltage/5*6)*0.003); //Average of 5 samples scaled to 30V full scale
return battVolt;
}

void setup(void)
{
  Serial.begin(9600);
  ads1015.begin();
}

void loop(void)
{
  
  battVolt = readBatt();
  Serial.print("AIN3:Battery Volts "); Serial.println(battVolt);
  Serial.println(" ");
  delay(1000);
}
