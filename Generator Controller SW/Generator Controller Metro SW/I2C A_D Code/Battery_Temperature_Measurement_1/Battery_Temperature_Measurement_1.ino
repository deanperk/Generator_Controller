
/*
 * Battery temperature Measurement 1
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
float battTemperature=0.0;
const int thermistorNominal = 10000;
const int temperatureNominal = 25;
const int bCoefficient = 3950;
const int seriesResistor = 10000;
float steinhart = 0.0;
float average=0.0; 


//Battery temperature measurement fuction
  float readBattTemp() {
  float battTemperature=0.0;
  unsigned int readTemperature = 0; 
   for (int i=0; i<5; i++) {
   readTemperature = readTemperature + ads1015.readADC_SingleEnded(1);
   delay(10);
   }  

 average = readTemperature/5; //average of 5 samples
  //convert reading to a resistance
 average = 1667 / average - 1;
 average = seriesResistor / average;
 // convert resistance to temperature
 steinhart = average / thermistorNominal;        // (R/Ro)
 steinhart = log(steinhart);                     // ln(R/Ro)
 steinhart /= bCoefficient;                      // 1/B * ln(R/Ro)
 steinhart += 1.0/(temperatureNominal + 273.15); // + (1/To)
 steinhart = 1.0/steinhart;                      // Invert
 steinhart -= 273.15;                            // convert to C
 steinhart = steinhart*9.0/5.0+32;               // convert to F
 battTemperature=steinhart;
 return battTemperature;

}
void setup(void)
{
  Serial.begin(9600);
  ads1015.begin();
}

void loop(void)
{
  
  battTemperature = readBattTemp();
  Serial.print("AIN1:Battery Temperature in degrees F "); Serial.println(battTemperature);
  Serial.println(" ");
  delay(1000);
}
