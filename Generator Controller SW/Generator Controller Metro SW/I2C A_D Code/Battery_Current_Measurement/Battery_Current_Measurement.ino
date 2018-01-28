/*
 * Battery Current Measurement
 */
#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1015 ads1015;
/* ADC Scaling
 *  12 bits or 4096 counts
 *  Single ended useage is 2048 counts
 *  Full scale is 6.144V or 3mV per bit
 *  5V measurement is 5/6.144 * 2048 = 1667bits
 *  1667 bits X 0.003 Volts per bit = 5V 
 */
// declare variables
   byte battCurrent=0;

//Define system battery charge current measurement function
  byte readBatt() {
  byte battCurrent=0;
  unsigned int readCurrent = 0; 
   for (int i=0; i<5; i++) {
   readCurrent = readCurrent + ads1015.readADC_SingleEnded(2);
   delay(10); //change to while loop using millis. do not use delay function
   }  
   battCurrent = ((readCurrent/5*20)*0.003); //Average of 5 samples scaled to 100A full scale
   return battCurrent;
}

void setup(void)
{
  Serial.begin(9600);
  ads1015.begin();
}

void loop(void)
{
  battCurrent = readBatt();
  Serial.print("AIN2:Battery Current "); Serial.println(battCurrent);
  Serial.println(" ");
  delay(1000);
}
