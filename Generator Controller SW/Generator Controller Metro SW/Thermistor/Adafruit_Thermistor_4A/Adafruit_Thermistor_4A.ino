

// Declare Variables
const int thermistorPin = A0;
const int thermistorNominal =  10000;
const int temperatureNominal = 25; 
const int numSamples = 5;
const int bCoefficient = 3950;
const int seriesResistor = 10000; 
int samples[numSamples];

 
void setup(void) {
  Serial.begin(9600);
  analogReference(EXTERNAL);
}
 
void loop(void) {
  byte i;
  float average;
 
  // take N samples in a row, with a slight delay then average them
  for (i=0; i< numSamples; i++) {
   samples[i] = analogRead(thermistorPin);
   delay(10);
  }
 
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
