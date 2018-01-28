#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1015 ads1015;
/* ADC Scaling]
 *  12 bits or 4096 counts
 *  Single ended useasge is 2048 counts
 *  Full scale is 6.144V or 3mV per bit
 *  5V measuerwment is 5/6.144 * 2048 = 1667bits
 *  1667 bits X 0.003 Volts per bit = 5V 
 */
// declare vaiables
float battTemperature=0.0;
const int thermistorNominal = 10000;
const int temperatureNominal = 25;
const int bCoefficient = 3950;
const int seriesResistor = 10000;
float steinhart;
float average=0;


//define fuctions
float readBatt() {
  float battTemperature=0.0;
  unsigned int readTemperature = 0; 
   for (int i=0; i<5; i++) {
   readTemperature = readTemperature + ads1015.readADC_SingleEnded(1);
   // Serial.print("readTemperature  = ");
   // Serial.println(readTemperature);
   delay(10);
   }  

 battTemperature = readTemperature/5;
 Serial.print("Battery Reading Average  = ");
 Serial.println(battTemperature);
 //battVolt = ((readVoltage/5*6)*0.003); //Average of 5 samples scaled to 30V full scale

 //convert reading to a resistance
 average = battTemperature; 
 Serial.print("averageA  = ");
 Serial.println(average);
 average = 1667 / average - 1;
 Serial.print("averageB  = ");
 Serial.println(average);
 average = seriesResistor / average;
 Serial.print("averageC  = ");
 Serial.println(average);
 battTemperature = average;
 // convert resistance to temperature
 steinhart = average / thermistorNominal;     // (R/Ro)
 steinhart = log(steinhart);                  // ln(R/Ro)
 steinhart /= bCoefficient;                   // 1/B * ln(R/Ro)
 steinhart += 1.0 / (temperatureNominal + 273.15); // + (1/To)
 steinhart = 1.0 / steinhart;                 // Invert
 steinhart -= 273.15;                         // convert to C
 steinhart = steinhart * 9.0 / 5.0 +32;         // convert to F
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
  
  battTemperature = readBatt();
  Serial.print("AIN1:Battery Temperature "); Serial.println(battTemperature);
  Serial.println(" ");
  delay(1000);
}
