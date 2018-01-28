/*
  Autonomous Power Solutions Rev 0.1
  Generator Powered Battery Charger
  
  
   Rev 0.1 is designed to run the S/W development prototype and may need updates before controlling an actual unit
 
   Port Assignments
   
   A0 = System Battery Voltage 0V to 30V range            chargeVolts
   A1 = System Charging Current 0A to 100A                chargeAmps
   A2 = Cranking Battery Voltage 0V to 15V range          crankVolts
   A3 = System Battery Temperature Monitor 10K Thermistor batteryTemp
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
 
 
 // Variables That Change
 float sensorValue = 0;
 float chargeVolts = 0;
 float chargeAmps = 0;
 float crankVolts = 0;
 float batteryTemp = 0;


 
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
 
 //End of Function Definitions *********************************
 
 // include the library code:
 #include "Wire.h"
 #include "Adafruit_LiquidCrystal.h"
 
 // Connect via i2c, default address #0 (A0-A2 not jumpered)
 Adafruit_LiquidCrystal lcd(0);
 
 
 /**************************************************************
                             Setup
  **************************************************************/
  
  void setup() {
    // set up the LCD's number of rows and columns: 
    lcd.begin(20,4);
    // Print a message to the LCD.
    lcd.setCursor(2,0);
    lcd.print("Autonomous Power");
    lcd.setCursor(7,1);
    lcd.print("Systems");
    lcd.setCursor(5,3);
    lcd.print("F/W Ver 0.1");
    analogReference(DEFAULT);
    Serial.begin(9600);
  }                           
 
 
 //End of setup loop *******************************************
 
 void loop() {
   delay(1000);
   //Read voltage on A0
   // sensorValue = readSensor(chargeVolts);
   sensorValue = analogRead(A3);
   Serial.print("sensorValue = ");
   Serial.println(sensorValue);
   chargeVolts = ((sensorValue / 1023) * 15);
   lcd.setCursor(0,2);
   lcd.print("Charge Volt =");
   lcd.print(chargeVolts);
   Serial.print("chargeVolts = ");
   Serial.println(chargeVolts);
 }
