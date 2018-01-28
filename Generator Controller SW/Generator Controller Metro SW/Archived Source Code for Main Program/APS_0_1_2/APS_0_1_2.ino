/*
  Autonomous Power Solutions Rev 0.1
  Generator Powered Battery Charger
  
  
   Rev 0.1 is designed to run the S/W development prototype and may need 
   updates before controlling an actual unit
 
   Port Assignments
   
   A0 = System Battery Voltage 0V to 30V range            voltagePin
   A1 = System Charging Current 0A to 100A                currentPin
   A2 = Cranking Battery Voltage 0V to 15V range          crankPin
   A3 = System Battery Temperature Monitor 10K Thermistor thermistorPin
   A4 = I2C Data
   A5 = I2C Clock
   
   D0 = Input Auto/Manual Toggle Switch Manual Position   manMode
   D1 = Input Auto/Manual Toggle Switch Auto Position     autoMode
   D2 = Input Rotary encoder for manual runtime selection??     runTime
   D3 = Input Crank/Run Signal from Comparator            runCrank
   D4 = Input Oil Level Switch from engine                oilAlarm
   D5 = Input Tachometer input (Freq Counter Lib)         engineRPM
   D6 = Input Start Switch Momentary Contact              engineStart
   D7 = Output Fault alarm                                faultAlarm
   D8 = Output Start Enable Relay                         startEnable
   D9 = Output Engine Start Relay                         startCommand
   D10 = Output Alternator Load Contactor                 loadControl
   D11 = SPI Bus for Data Logging 
   D12 = SPI Bus for Data Logging 
   D13 = SPI Bus for Data Logging   
   
 
 */ 


 /* ----------------------------------------------------------
                      Define Variables 
    ---------------------------------------------------------- */                  
 
 // Variables That Do Not Change
 const int debounceDelay = 10; // number of mS delay used in debounce function
 const int manMode = 0;
 const int autoMode = 1;
 const int runCrank = 3;
 const int oilAlarm = 4;
 const int engineRPM = 5;
 const int engineStart = 6;
 const int faultAlarm = 7;
 const int startEnable = 8;
 const int startCommand = 9;
 const int loadControl = 10;
 const int voltagePin = A0;
 const int currentPin = A1;
 const int crankPin = A2;
 const int thermistorPin = A3;
 const int thermistorNominal =  10000;
 const int temperatureNominal = 25; 
 const int bCoefficient = 3950;
 const int seriesResistor = 10000; 
 
 // Variables That Change
 float sensorValue = 0;
 float chargeVolts = 0;
 float chargeAmps = 0;
 float crankVolts = 0;
 float batteryTempF = 0;
 float steinhart = 0;
 
 // End of Variables Definition ******************************
  
  
  
  
 /* ----------------------------------------------------------
                      Function Definitions 
    ---------------------------------------------------------- */
 
 // Debounce Function
 boolean debounce(int pin)
 {
   boolean state;
   boolean previousState;
  
   previousState = digitalRead(pin); // store switch state
   for(int counter=0; counter < debounceDelay; counter++)
   {
    delay(1); //wait for 1 ms
    state = digitalRead(pin); //read the pin
    if(state != previousState)
    {
     counter = 0; //reset the counter if the state changes
     previousState = state; // and save the current state 
    }
   }   
 }
 
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


// thermistor convert function
  float thermistorConvert(float sensorValue){
  sensorValue = 1023 / sensorValue - 1;
  sensorValue = seriesResistor / sensorValue;
  steinhart = sensorValue / thermistorNominal;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= bCoefficient;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (temperatureNominal + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart; // Invert
  steinhart -= 273.15;  // convert to C
  steinhart = steinhart * 9.0 / 5.0 +32;         // convert to F
  batteryTempF = steinhart;
  return batteryTempF;
}
 
 //End of Function Definitions *********************************
 
 // include the library code
 #include "Wire.h"
 #include "Adafruit_LiquidCrystal.h"
 Adafruit_LiquidCrystal lcd(0); // Connect via i2c, default address #0 (A0-A2 not jumpered)
 
 
 /**************************************************************
                             Setup
  **************************************************************/
  
  void setup() {
    analogReference(DEFAULT); //THIS WILL CAHNGE TO EXTERNAL REF IN HARDWARE DESIGN
    Serial.begin(9600);
    // set up lcd and and send welcome message
    lcd.begin(20,4); // set up the LCD's number of rows and columns: 
    lcd.setCursor(2,0); // Print opening message to the LCD.
    lcd.print("Autonomous Power");
    lcd.setCursor(7,1);
    lcd.print("Systems");
    lcd.setCursor(5,3);
    lcd.print("F/W Ver 0.1");
    delay (2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Charge Volts=  ");
    lcd.setCursor(0,1);
    lcd.print("Charge Amps = ");
    lcd.setCursor(0,2);
    lcd.print("Crank  Volts= ");
    lcd.setCursor(0,3);
    lcd.print("Temperature = ");
    // set up digital I/O
    pinMode(manMode,INPUT); //set up Auto/Manual select switch
    digitalWrite(manMode,HIGH); // internal pull up on
    pinMode(autoMode,INPUT); //set up Auto/Manual select switch
    digitalWrite(autoMode,HIGH); // internal pull up on
    pinMode(runCrank,INPUT); //set up run/crank comparator  
    pinMode(engineRPM,INPUT); //set up tachometer input 
    pinMode(engineStart,INPUT); //set up engine mom start switch
    digitalWrite(engineStart,HIGH); // internal pull up on
    pinMode(faultAlarm,OUTPUT); // set up Fault Alarm output
    digitalWrite(faultAlarm,LOW); //turn off 
    pinMode(startEnable,OUTPUT); // set up Start Enable output
    digitalWrite(startEnable,LOW); //turn off 
    pinMode(startCommand,OUTPUT); // set up Fault Alarm output
    digitalWrite(startCommand,LOW); //turn off 
    pinMode(loadControl,OUTPUT); // set up Fault Alarm output
    digitalWrite(loadControl,LOW); //turn off 
  
  }                           
 
 
 //End of setup loop *******************************************
 
 void loop() {
   delay(1000);
   sensorValue = readSensor(voltagePin); // Read system battery voltage
   chargeVolts = ((sensorValue / 1023) * 5 * 6);  // scale output to 30V
   lcd.setCursor(14,0);
   lcd.print(chargeVolts);
   
   sensorValue = readSensor(currentPin); // Read system battery current
   chargeAmps = ((sensorValue / 1023) * 5 * 20);  // scale output to 100A
   lcd.setCursor(14,1);
   lcd.print(chargeAmps);

   sensorValue = readSensor(crankPin); // Read cranking battery voltage
   crankVolts = ((sensorValue / 1023) * 5 * 3);  // scale output to 15V
   lcd.setCursor(14,2);
   lcd.print(crankVolts);

   sensorValue = readSensor(thermistorPin); //Read system battery temperature
   thermistorConvert(sensorValue);
   lcd.setCursor(14,3);
   lcd.print(batteryTempF);  
 }
