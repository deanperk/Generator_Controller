
// Dean Thermistor 4C 2/20/16

// Declare Variables
const int thermistorPin = A3;
const int thermistorNominal =  10000;
const int temperatureNominal = 25; 
const int bCoefficient = 3950;
const int seriesResistor = 10000; 
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
  analogReference(DEFAULT);
}
 
void loop(void) {
  sensorValue = readSensor(thermistorPin);
  Serial.print("sensorValue after function return   ");
  Serial.println(sensorValue);
  sensorValue = 1023 / sensorValue - 1;
  Serial.print("sensorValue after first calc   ");
  Serial.println(sensorValue);
  sensorValue = seriesResistor / sensorValue;
  Serial.print("sensorValue after second calc   ");
  Serial.println(sensorValue);
  steinhart = sensorValue / thermistorNominal;     // (R/Ro)
  Serial.print("steinhart after third calc   ");
  Serial.println(steinhart);
  steinhart = log(steinhart);                  // ln(R/Ro)
  Serial.print("steinhart after forth calc   ");
  Serial.println(steinhart);
  steinhart /= bCoefficient;                   // 1/B * ln(R/Ro)
  Serial.print("steinhart after fifth calc   ");
  Serial.println(steinhart);
  steinhart += 1.0 / (temperatureNominal + 273.15); // + (1/To)
  Serial.print("steinhart after sixth calc   ");
  Serial.println(steinhart);
  steinhart = 1.0 / steinhart; // Invert
  Serial.print("steinhart after seventh calc   ");
  Serial.println(steinhart);
  steinhart -= 273.15;  // convert to C
  Serial.print("steinhart after eighth calc   ");
  Serial.println(steinhart);
  steinhart = steinhart * 9.0 / 5.0 +32;         // convert to F
    
  Serial.print("Temperature "); 
  Serial.print(steinhart);
  Serial.println(" *F");
  delay(1000);
}
