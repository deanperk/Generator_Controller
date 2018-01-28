#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1015 ads1015;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV)");
  ads1015.begin();
}

void loop(void)
{
  float adc0, adc1, adc2, adc3;

  adc0 = ads1015.readADC_SingleEnded(0);
  adc1 = ads1015.readADC_SingleEnded(1);
  adc2 = ads1015.readADC_SingleEnded(2)*0.060;
  adc3 = ads1015.readADC_SingleEnded(3) * 0.018;
  Serial.print("AIN0:Amb Temp "); Serial.println(adc0);
  Serial.print("AIN1:Battery Temp "); Serial.println(adc1);
  Serial.print("AIN2:Charge Amps "); Serial.println(adc2);
  Serial.print("AIN3:Battery Volts "); Serial.println(adc3);
  Serial.println(" ");
  
  delay(1000);
}
