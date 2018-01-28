/*
  Autonomous Power Solutions Rev 0.1.3.2
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
 *                      Define Global Variables 
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
 const int seriesResistor=10000;//nominal value,may be measured for better accuracy
 
 //VARIABLES THAT CHANGE
 float sensorValue = 0.0; 
 float battVolt = 0.0;          //main system battery bank voltage 
 byte  battCurrent = 0;         //main system battery bank charge current
 float crankVolts = 0.0;        //engine crank battery voltage
 float battTemperature = 0.0;   //main system battery temperature
 float ambTemperature = 0.0;    //main system battery enclosure temperature
 float steinhart = 0.0;
 float average=0.0;             //WHY IS THIS A FLOAT?
 int runTime = 0;               //manual mode run time  
 int bcdTotal=0;
 int engineRPM=0;               //engine tachometer 0 to 3600 RPM
 volatile int mode=3;           //Used in BCD run time selection can this be data type "byte"?
 volatile int total=0;          //set to OFF state for power up can this be data type "byte"?
 
 // End of Variables Definition -------------------------
  
 

 /* ----------------------------------------------------------
                      Function Definitions 
    ---------------------------------------------------------- */
 
 
 //READSENSOR FUNCTION USED IN CRANK BATTERY VOLTAGE MEASUREMENT
  float readSensor(int sensorPin) {
   // float sensorValue = 0.0; // already declared as a global variable
   sensorValue = 0.0;  // initialize to 0.0
   for (int i=0; i<5; i++) {
     sensorValue = sensorValue + analogRead(sensorPin);
     delay(10);
   }
     crankVolts = ((sensorValue/5)*0.00488*3); //5V full scale / 1024 bits = 0.00488 v/bit
     return crankVolts;
 }   

 //SYSTEM BATTERY VOLTAGE MEASUREMENT FUNCTION
  float readBattVolt() {
   //float battVolt=0.0; // already declared as a global vaiable - can this be local?
   battVolt = 0.0; // initialize to 0.0 
   // float readVoltage = 0.0; // unnecessary variable
    for (int i=0; i<5; i++) {
    // readVoltage = readVoltage + ads1015.readADC_SingleEnded(3);
    battVolt = battVolt + ads1015.readADC_SingleEnded(3);
    delay(10);
   }  
  battVolt = ((battVolt/5*6)*0.003); //Average of 5 samples scaled to 30V full scale
  Serial.println(battVolt);
  return battVolt;
}


 
 /* 
  *  ----------------------------------------------------------
  *                            SETUP
  * ----------------------------------------------------------                           
  */
  
  void setup() {
    analogReference(DEFAULT);          //THIS WILL CHANGE TO EXT REF IN H/W DESIGN
    Serial.begin(9600);                //THIS CAN BE REMOVED AFTER TESTING
    ads1015.begin();                   // Create instance of ADS1015 at default address
    // set up lcd and and send welcome message
    lcd.begin(20,4);                   // set up the LCD's number of rows and columns: 
    lcd.setCursor(2,0);                // Print opening message to the LCD.
    lcd.print("Autonomous Power");
    lcd.setCursor(7,1);
    lcd.print("Systems");
    lcd.setCursor(5,3);
    lcd.print("F/W Ver 0.1.3.2");
    delay (2000);                      // CHANGE TO CODE THAT DOES NOT USE A DELAY() FUNCTION
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Charge Volts=  ");
    lcd.setCursor(0,1);
    lcd.print("Charge Amps = ");
    lcd.setCursor(0,2);
    lcd.print("Crank  Volts= ");
    lcd.setCursor(0,3);
    lcd.print("T= ");
    lcd.setCursor(8,3);
    lcd.print("RPM = ");
    
    // set up digital I/O
    pinMode(manual,INPUT);             //set up Auto/Manual select switch
    digitalWrite(manual,HIGH);         //internal pull up on
    pinMode(automatic,INPUT);          //set up Auto/Manual select switch
    digitalWrite(automatic,HIGH);      //internal pull up on
    pinMode(engineSpeed,INPUT);        //set up tachometer input 
    pinMode(engineStart,INPUT);        //set up engine mom start switch
    digitalWrite(engineStart,HIGH);    //internal pull up on
    pinMode(faultAlarm,OUTPUT);        //set up Fault Alarm output
    digitalWrite(faultAlarm,LOW);      //turn off 
    pinMode(startEnable,OUTPUT);       //set up Start Enable output
    digitalWrite(startEnable,LOW);     //turn off 
    pinMode(startCommand,OUTPUT);      //set up Start Relay output
    digitalWrite(startCommand,LOW);    //turn off 
    pinMode(loadControl,OUTPUT);       //set up Load Contactor output
    digitalWrite(loadControl,LOW);     //turn off 
    pinMode(q1,INPUT);                 //set up BCD runtime select switch
    digitalWrite(q1,HIGH);             //internal pull up on
    pinMode(q2,INPUT);                 //set up BCD runtime select switch
    digitalWrite(q2,HIGH);             //internal pull up on
    pinMode(q4,INPUT);                 //set up BCD runtime select switch
    digitalWrite(q4,HIGH);             //internal pull up on   
   
  }                           
 //End of setup loop ------------------------------------------------

 void loop() {
   battVolt = readBattVolt();
   Serial.println(battVolt);
   lcd.setCursor(14,0);
   lcd.print(battVolt);
   delay(1000);
 }
