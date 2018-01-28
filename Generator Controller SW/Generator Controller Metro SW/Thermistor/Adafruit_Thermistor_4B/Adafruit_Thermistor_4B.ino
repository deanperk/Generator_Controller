
// Dean Thermistor 4B 2/20/16

// Declare Variables
const int thermistorPin = A3;
const int thermistorNominal =  10000;
const int temperatureNominal = 25; 
// const int numSamples = 5;
const int bCoefficient = 3950;
const int seriesResistor = 10000; 
// int samples[numSamples];
float sensorValue = 0;
float steinhart = 0;

// Define Functions ******************************************

//readSensor function
 float readSensor(int sensorPin) {
   float sensorValue = 0;
   for (int i=0; i<5; i++) {
     sensorValue = sensorValue + analogRead(sensorPin);
     delay(10);
   }
   sensorValue = constrain(sensorValue, 0, 5115);
   sensorValue = map(sensorValue, 0, 5115, 0, 1023);
   return sensorValue;
 }   
 
 //End of Function Definitions *********************************
 
void setup(void) {
  Serial.begin(9600);
  analogReference(EXTERNAL);
}
 
void loop(void) {
  sensorValue = readSensor(thermistorPin);
  sensorValue = 1023 / sensorValue - 1;
  sensorValue = seriesResistor / sensorValue;
  steinhart = sensorValue / thermistorNominal;     // (R/Ro)
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
