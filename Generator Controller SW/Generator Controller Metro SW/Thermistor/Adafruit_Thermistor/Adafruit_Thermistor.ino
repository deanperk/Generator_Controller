// which analog pin to connect
const int thermistorPin = A0;         
// resistance at 25 degrees C
const int thermistorNominal =  10000;      
// temp. for nominal resistance (almost always 25 C)
const int temperatureNominal = 25; 
// how many samples to take and average, more takes longer
// but is more 'smooth'
const int numSamples = 5;
// The beta coefficient of the thermistor (usually 3000-4000)
const int bCoefficient = 3950;
// the value of the 'other' resistor
const int seriesResistor = 10000;    
 
int samples[numSamples];
 
void setup(void) {
  Serial.begin(9600);
  analogReference(EXTERNAL);
}
 
void loop(void) {
  uint8_t i;
  float average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< numSamples; i++) {
   samples[i] = analogRead(thermistorPin);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< numSamples; i++) {
     average += samples[i];
  }
  average /= numSamples;
  

  // convert the value to resistance
  average = 1023 / average - 1;
  average = seriesResistor / average;

 
  float steinhart;
  steinhart = average / thermistorNominal;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= bCoefficient;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (temperatureNominal + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  steinhart = steinhart * 9.0 / 5.0 +32;         // convert to F
    
  
  
  Serial.print("Temperature "); 
  Serial.print(steinhart);
  Serial.println(" *F");
 
  delay(1000);
}
