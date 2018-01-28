/*
  Autonomous Power Solutions Rev 0.1.3.3
  Generator Powered Battery Charger
  
  
   Rev 0.1.3.2 is designed to run on the S/W development prototype PCA
   and will need minor updates before controlling an actual unit
 
   Port Assignments
   
   A0 =           
   A1 =              
   A2 =         
   A3 = Cranking Battery Voltage 0V to 15V range          crankPin
   A4 = I2C Data
   A5 = I2C Clock

   I2C A0 System Ambient Temperature                      AIN0
   I2C A1 System Battery Temperature                      AIN1
   I2C A2 System Charging Current 0A to 100A              AIN2
   I2C A3 System Charging Voltage 0V to 30V range         AIN3
   
   D0 = Input Oil Level Switch from engine                oilAlarm
   D1 = Input Start Switch Momentary Contact              engineStart
   D2 = Input Auto/Manual Toggle Switch Manual Position   manMode
   D3 = Input Auto/Manual Toggle Switch Auto Position     autoMode
   D4 = Output Auxilary Load Control                      auxControl
   D5 = Input Tachometer input                            engineSpeed
   D6 = Output Fan On/Off                                 fanControl
   D7 = Output Engine Start Relay                         startCommand
   D8 = Output Alternator Load Contactor                  loadControl
   D9 = Output Fault alarm                                faultAlarm
   D10 = Output Start Enable Relay                        startEnable
   D11 = Manual Runtime Switch BCD1                       q1
   D12 = Manual Runtime Switch BCD2                       q2 
   D13 = Manual Runtime Switch BCD4                       q4   
 */ 

/*-------------------------------------------------------------
*                        INCLUDES
*-------------------------------------------------------------*/
 
 #include <Wire.h>
 #include <Adafruit_ADS1015.h>
 #include <Adafruit_LiquidCrystal.h>
 Adafruit_LiquidCrystal lcd(0); // Default address #0 A0-A2 not jumpered
 Adafruit_ADS1015 ads1015; 

 /*----------------------------------------------------------
 *                      Define Variables 
 *----------------------------------------------------------- */                  

 //VARIABLES THAT DO NOT CHANGE
 const int debounceDelay = 10;  // number of mS delay used in debounce function
 const int oilAlarm = 0;
 const int engineStart = 1;
 const int manual = 2;
 const int automatic = 3;
 const int auxControl = 4;
 const int engineSpeed = 5;
 const int fanControl = 6;
 const int faultAlarm = 7;
 const int startEnable = 8;
 const int startCommand = 9;
 const int loadControl = 10;
 const int q1 = 11;
 const int q2 = 12;
 const int q4 = 13;
 const int crankPin = A3;
 const int thermistorNominal=10000;
 const int temperatureNominal=25; 
 const int bCoefficient=3950;
 const int seriesResistor=10000;//nominal value,measure for better accuracy
 
 //VARIABLES THAT CHANGE
 float sensorValue = 0.0; 
 float battVolt = 0.0;       //main system battery bank voltage 
 float battCurrent = 0.0;    //main system battery bank charge current
 float crankVolts = 0.0;     //engine crank battery voltage
 float battTemperature = 0;  //main system battery temperature
 float ambTemperature = 0.0; //main system battery enclosure temperature
 float steinhart = 0.0;
 int runTime = 0;            //manual mode run time  
 int bcdTotal=0;
 int engineRPM =0;           //engine tachometer 0 to 3600 RPM
 volatile int mode=3;        //Used in BCD run time selection-data type "byte"?
 volatile int total=0;       //set to OFF state for power up-data type "byte"?
 
 // End of Variables Definition -------------------------
  
 

 /* ----------------------------------------------------------
                      Function Definitions 
    ---------------------------------------------------------- */
 
 //DEBOUNCE FUNCTION
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
 
 //READSENSOR FUNCTION USED IN CRANK BATTERY VOLTAGE MEASUREMENT
  float readSensor(int sensorPin) {
  sensorValue = 0.0; //Initialize sensorValue
   for (int i=0; i<5; i++) {
     sensorValue = sensorValue + analogRead(sensorPin);
     delay(10);
   }
     crankVolts = ((sensorValue/5)*0.0048*3); //scaled to 15V full scale
     return crankVolts;
 }   

 //SYSTEM BATTERY VOLTAGE MEASUREMENT FUNCTION
  float readBattVolt() {
  battVolt = 0.0; //Initialize battVolt
   for (int i=0; i<5; i++) {
   battVolt = battVolt + ads1015.readADC_SingleEnded(3);
   delay(10);
   }  
  battVolt = ((battVolt/5*6)*0.003); //Average of 5 samples scaled to 30V full scale
  return battVolt;
}

 //SYSTEM BATTERY CHARGE CURRENT MEASUREMENT FUNCTION
  float readBattAmps() {
  battCurrent=0.0; //Initialize battCurrent
    for (int i=0; i<5; i++) {
    battCurrent = battCurrent + ads1015.readADC_SingleEnded(2);
    delay(10); //change to while loop using millis. do not use delay function
   }  
   battCurrent = ((battCurrent/5*20)*0.003); //Average of 5 samples scaled to 100A full scale
   return battCurrent;
}

//BATTERY TEMPERATURE MEASUREMENT FUNCTION
  float readBattTemp() {
  steinhart = 0.0;
  battTemperature=0.0; // initialize battTemperature
   for (int i=0; i<5; i++) {
   battTemperature = battTemperature + ads1015.readADC_SingleEnded(1);
   delay(10);
   }  
 battTemperature=battTemperature/5; //average of 5 samples
 //convert reading to a resistance
 battTemperature=1667 / battTemperature - 1;
 battTemperature=seriesResistor / battTemperature;
 //convert resistance to temperature
 steinhart=battTemperature/thermistorNominal;        // (R/Ro)
 steinhart=log(steinhart);                   // ln(R/Ro)
 steinhart/=bCoefficient;                    // 1/B * ln(R/Ro)
 steinhart+=1.0/(temperatureNominal+273.15); // + (1/To)
 steinhart=1.0/steinhart;                    // Invert
 steinhart-=273.15;                          // convert to C
 steinhart=steinhart*9.0/5.0+32;             // convert to F
 battTemperature=steinhart;
 return battTemperature;
}

//BATTERY ENCLOSURE AMBIENT TEMPERATURE MEASUREMENT FUNCTION
  float readAmbTemp() {
  steinhart=0.0;
  ambTemperature=0.0; // initialize ambTemperature
   for (int i=0; i<5; i++) {
   ambTemperature = ambTemperature + ads1015.readADC_SingleEnded(0);
   delay(10);
   }  
 ambTemperature=ambTemperature/5; //average of 5 samples
 //convert reading to a resistance
 ambTemperature=1667 / ambTemperature - 1;
 ambTemperature=seriesResistor / ambTemperature;
 //convert resistance to temperature
 steinhart=ambTemperature/thermistorNominal;        // (R/Ro)
 steinhart=log(steinhart);                   // ln(R/Ro)
 steinhart/=bCoefficient;                    // 1/B * ln(R/Ro)
 steinhart+=1.0/(temperatureNominal+273.15); // + (1/To)
 steinhart=1.0/steinhart;                    // Invert
 steinhart-=273.15;                          // convert to C
 steinhart=steinhart*9.0/5.0+32;             // convert to F
 ambTemperature=steinhart;
 return ambTemperature;
}


// ENGINE RPM FUNCTION GOES HERE

//BCD Run Time Switch Function 
int readSwitch()
{
 int bcdTotal=0;
 runTime=0;
 if (digitalRead(q1)==HIGH) { bcdTotal+=1; }
 if (digitalRead(q2)==HIGH) { bcdTotal+=2; }
 if (digitalRead(q4)==HIGH) { bcdTotal+=4; }
 // Set manual run time based on BCD switch input
 switch (bcdTotal){
    case 0:
      runTime = 2; //2 hours of manaual runtime
      break;
    case 1:
      runTime = 4;
      break;
    case 2:
      runTime = 6;
      break;    
    case 3:
      runTime = 8;
      break;
    case 4:
      runTime = 10;
      break; 
    case 5:
      runTime = 12;
      break; 
    default: 
      runTime = 0;
      break;
 }
 return runTime;
}
 
//End of Function Definitions *********************************
 
 

/*
 * --------------------------------------------------------
 * ISRs
 * ---------------------------------------------------------
*/ 

//Manual/Auto Mode Interrupt Service Routine (ISR)
void switchPressed1()
{
 volatile int total=0;
 if (digitalRead(manual)==HIGH) { total+=1; }
 if (digitalRead(automatic)== HIGH) { total+=2; }

 switch (total) {
  case 0:
  mode=0; //fault mode
  break;
  case 1:
  mode=1; //automatic mode
  break;
  case 2:
  mode=2; //manual mode
  break;
  case 3:
  mode=3; //off mode
  break;
 } 
}
 
 
 // END OF ISRs

 
 /* 
  *  ----------------------------------------------------------
  *                            SETUP
  * ----------------------------------------------------------                           
  */
  
  void setup() {
    analogReference(DEFAULT);          //THIS WILL CHANGE TO EXT REF IN H/W DESIGN
    Serial.begin(9600);                //THIS CAN BE REMOVED AFTER TESTING
    ads1015.begin();
    // set up lcd and and send welcome message
    lcd.begin(20,4);                   // set up the LCD's number of rows and columns: 
    lcd.clear();
    lcd.setCursor(2,0);                // Print opening message to the LCD.
    lcd.print("Autonomous Power");
    lcd.setCursor(7,1);
    lcd.print("Systems");
    lcd.setCursor(3,3);
    lcd.print("F/W Ver 0.1.3.3");
    delay (2000);                     // CHANGE TO CODE THAT DOES NOT USE A DELAY() FUNCTION
        
    // set up digital I/O
    pinMode(manual,INPUT);            //set up Auto/Manual select switch
    digitalWrite(manual,HIGH);        //internal pull up on
    pinMode(automatic,INPUT);         //set up Auto/Manual select switch
    digitalWrite(automatic,HIGH);     //internal pull up on
    pinMode(engineSpeed,INPUT);       //set up tachometer input 
    pinMode(engineStart,INPUT);       //set up engine mom start switch
    digitalWrite(engineStart,HIGH);   //internal pull up on
    pinMode(faultAlarm,OUTPUT);       //set up Fault Alarm output
    digitalWrite(faultAlarm,LOW);     //turn off 
    pinMode(startEnable,OUTPUT);      //set up Start Enable output
    digitalWrite(startEnable,LOW);    //turn off 
    pinMode(startCommand,OUTPUT);     //set up Start Relay output
    digitalWrite(startCommand,LOW);   //turn off 
    pinMode(loadControl,OUTPUT);      //set up Load Contactor output
    digitalWrite(loadControl,LOW);    //turn off 
    pinMode(q1,INPUT);                //set up BCD runtime select switch
    digitalWrite(q1,HIGH);            //internal pull up on
    pinMode(q2,INPUT);                //set up BCD runtime select switch
    digitalWrite(q2,HIGH);            //internal pull up on
    pinMode(q4,INPUT);                //set up BCD runtime select switch
    digitalWrite(q4,HIGH);            //internal pull up on   
    //set up interrupts
    attachInterrupt (0, switchPressed1, CHANGE);  // attach Auto/Manual ISR
    attachInterrupt (1, switchPressed1, CHANGE);  // attach Auto/Manual ISR
  
 /* DETERMINE MODE AND SET UP DISPLAY ACCORDINGLY
     READ MODE SWITCH MANUALLY (NOT ISR)
     IF "OFF" - DISPLAY IS BLANK BUT WE WILL MONITOR 
     TEMPERATURE AND VOLTAGE FOR FAULT CONDITIONS

     IF "MANUAL" ENABLE SE RELAY ANDWAIT FOR START SWITCH
     IF "AUTOMATIC" ENABLE SE RELAY AND WAIT FOR VOLTAGE THRESHOLD
    
  */
  
                           
 
 //End of setup loop ------------------------------------------------

 void loop() {
  
   battVolt = readBattVolt();
   lcd.setCursor(14,0);
   lcd.print(battVolt);
   
   battCurrent = readBattAmps();
   lcd.setCursor(14,1);
   lcd.print(battCurrent);

   readSensor(A3);
   lcd.setCursor(14,2);
   lcd.print(crankVolts);

   battTemperature = readBattTemp();
   lcd.setCursor(4,3);
   lcd.print(battTemperature);  
 
   ambTemperature = readAmbTemp();
   lcd.setCursor(14,3);
   lcd.print(ambTemperature);
 
   Serial.print("Mode=");
   Serial.println(mode); 

   runTime = readSwitch();
   Serial.print("RunTime=");
   Serial.println(runTime);

   delay(1000);
 }
