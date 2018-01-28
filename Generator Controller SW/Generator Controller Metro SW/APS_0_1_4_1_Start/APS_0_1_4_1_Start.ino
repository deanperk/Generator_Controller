
/*
  Autonomous Power Solutions Rev 0.1.4.0
  Generator Powered Battery Charger
  
  
   Rev 0.1.4.0 is designed to run on the S/W development prototype PCA
   and will need minor updates before controlling an actual unit
 
   PORT ASSIGNMENTS
   
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
   
   D0 = Input Oil Level Switch from engine                oilSwitch
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
 

  FAN OPERATION
  Battery temperature of 50F to 80F is safe range. No action required.
  Above 80F, batteries will be cooled by circulating air. 
  Ambient air temperature should be >=5 degrees less than battery to 
  effectively cool the battery.
  If the battery temperature is >=100 F, the battery is over temperature
  and a temperature fault condition exits. Charging and should be
  disallowed until the battery is below 100F.  
 
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
 const int oilSwitch = A0;
 const int engineStart = A1;
 const int manual = 2;
 const int automatic = 3;
 const int auxControl = 4;
 const int engineSpeed = 5;
 const int fanControl = 6;
 const int faultAlarm = 9;
 const int startEnable = 8;
 const int startCommand = 7;
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
 float battTemperature = 0.0;  //main system battery temperature
 float ambTemperature = 0.0; //main system battery enclosure temperature
 float steinhart = 0.0;
 int runTime = 0;            //manual mode run time value
 int bcdTotal=0;             //manual mode run time selection switch
 int engineRPM =0;           //engine tachometer 0 to 3600 RPM
 unsigned long startTime=0;  //used in 2 minute time out for warm up period  
 //look up millis timer code in TAM test fixture 
 unsigned long warmUpTime=0; //2 minute warmup period
 unsigned long countHour=0;  //counter used by timer interrupt routine
 volatile int mode=3;        //set to OFF state for power up-data type "byte"?
 volatile int modeTotal=0;   //Used in BCD run time selection-data type "byte"?
 boolean engineRun = false;  //engine RPM > 800 
 boolean oilFault = false;   //oil level switch is closed
 boolean voltFault = false;  //battery under voltage
 boolean tempFault = false;  //battery over temperature 
 boolean fanOn = false;      //fan signal is ON
 boolean loadOn = false;     //load contactor is connected
 boolean startFault = false; //no start after 5 attempts
 boolean currentFault = false;//under or over current condition
 boolean criticalFault = false; //a fault that will prevent engine from running
 // int faultCondition = 0;  DETERMINE IF THIS SHOULD BE BOOLEAN TRUE/FALSE
 // OR AN INT WHOSE VALUE DEFINES FAULT TYPE - EG 0 FOR NO FAULT, 1 FOR VOLT FAULT ETC

 //String Variables
 String firmwareVer="0.1.4.0";
 String sysBattFault="Batt Fault|Sys Fault";
 String voltageFault="Volt";
 String temperatureFault="Temp";
 String oilLevelFault="Oil Level";
 String ampFault="Current";
 String crank="Crank Battery";
 String chargeV="Charge Volts = ";
 String chargeI="Charge Amps = ";
 String battT="Battery Temp = ";
 String ambT="Ambient Temp = ";
 String warmUp="Engine Warm Up";
 
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


//ENGINE RPM FUNCTION GOES HERE
//FAULT STATUS FUNCTION GOES HERE
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
      runTime = 2; //2 hours of manual runtime
      break;
    case 1:
      runTime = 4; //SHOULD THESE TIMES BE IN MINUTES?
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
 volatile int modeTotal=0;
 if (digitalRead(manual)==HIGH) { modeTotal+=1; }
 if (digitalRead(automatic)== HIGH) { modeTotal+=2; }

 switch (modeTotal) {
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
    lcd.begin(20,4);                   // set up the LCD's number of rows and columns
    Serial.println("entering setup loop");
    Serial.println(" ");
    //SET UP DIGITAL I/O
    pinMode(manual,INPUT_PULLUP);      //set up Auto/Manual select switch
    // digitalWrite(manual,HIGH);      //internal pull up on
    pinMode(automatic,INPUT_PULLUP);   //set up Auto/Manual select switch
    //digitalWrite(automatic,HIGH);    //internal pull up on
    pinMode(engineSpeed,INPUT);        //set up tachometer input 
    pinMode(engineStart,INPUT_PULLUP); //set up engine mom start switch
    //digitalWrite(engineStart,HIGH);  //internal pull up on
    pinMode(faultAlarm,OUTPUT);        //set up Fault Alarm output
    digitalWrite(faultAlarm,LOW);      //turn off 
    pinMode(startEnable,OUTPUT);       //set up Start Enable output
    digitalWrite(startEnable,LOW);     //turn off 
    pinMode(startCommand,OUTPUT);      //set up Start Relay output
    digitalWrite(startCommand,LOW);    //turn off 
    pinMode(loadControl,OUTPUT);       //set up Load Contactor output
    digitalWrite(loadControl,LOW);     //turn off 
    pinMode(oilSwitch,INPUT_PULLUP);   //set up oil level switch
    //digitalWrite(oilSwitch,HIGH);    //internal pull up on
    pinMode(q1,INPUT_PULLUP);          //set up BCD runtime select switch
    //digitalWrite(q1,HIGH);           //internal pull up on
    pinMode(q2,INPUT_PULLUP);          //set up BCD runtime select switch
    //digitalWrite(q2,HIGH);           //internal pull up on
    pinMode(q4,INPUT_PULLUP);          //set up BCD runtime select switch
    //digitalWrite(q4,HIGH);           //internal pull up on   
    pinMode(fanControl,OUTPUT);        //set up Load Contactor output
    digitalWrite(fanControl,LOW);      //turn off 
    pinMode(auxControl,OUTPUT);        //set up Load Contactor output
    digitalWrite(auxControl,LOW);      //turn off 

    
    //set up interrupts
    attachInterrupt (0, switchPressed1,CHANGE);  // attach Auto/Manual ISR
    attachInterrupt (1, switchPressed1,CHANGE);  // attach Auto/Manual ISR

    //set up lcd and and send welcome message 
    lcd.clear();
    delay(1000); //remove aFTER TESTING
    lcd.setCursor(2,0);        // Print opening message to the LCD
    lcd.print("Autonomous Power");
    lcd.setCursor(7,1);
    lcd.print("Systems");
    lcd.setCursor(3,3);
    lcd.print("F/W Ver");
    lcd.setCursor(11,3);
    lcd.print(firmwareVer);
    delay (2000);
    

   //DETERMINE OPERATING MODE -  MAKE THIS A FUNCTION?
   //CHECK FOR CONFLICT WITH VARIABLES HERE ALSO BEING USED IN ISR CODE
   //VARIABLES ARE VOLATILE IN ISR CODE BUT NOT VOLATILE HERE
   modeTotal=0;
   if (digitalRead(manual)==HIGH) { modeTotal+=1; }
   if (digitalRead(automatic)== HIGH) { modeTotal+=2; }
   switch (modeTotal) {
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
     
   //DETERMINE IF THERE IS A MODE SWITCH FAULT CONDITION
   //ELIMINATE MODE SWITCH FAULT CHECK 
   /*if (mode == 0) {
      modeFault = true;
      }
    
    if (modeFault = true) { 
       digitalWrite(faultAlarm,HIGH);  //set Fault Alarm contact closure
       //lcd.clear();                  //determine when to clear the lcd
       lcd.setCursor(0,3)
       lcd.print(modeSwitchFault);
       }
     */
  
   //DETERMINE IF A BATTERY VOLTAGE FAULT EXISTS
   battVolt = readBattVolt();
   if (battVolt <=22.0) {
      voltFault = true;
      digitalWrite(faultAlarm,HIGH);  //set Fault Alarm contact closure
      }
 
   //DETERMINE IF A BATTERY TEMPERATURE FAULT EXISTS
   battTemperature = readBattTemp();
   if (battTemperature >=100.0) {
       tempFault = true;
       digitalWrite(faultAlarm,HIGH);  //set Fault Alarm contact closure
       digitalWrite(startEnable,LOW);  //prevent engine start
       }

   //DETERMINE IF OIL LEVEL FAULT EXISTS
   if (digitalRead(oilSwitch) == LOW){
       oilFault = true;
       Serial.print("oilFault= ");
       Serial.println(oilFault);   
       digitalWrite(faultAlarm,HIGH);  //set Fault Alarm contact closure
       digitalWrite(startEnable,LOW);  //prevent engine start
       }

   //SET UP FAULT DISPLAY
   if ((voltFault == true) || (oilFault == true) || (tempFault == true)){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(sysBattFault);
        lcd.setCursor(10,1);
        lcd.print("|");
        lcd.setCursor(10,2);
        lcd.print("|");
        lcd.setCursor(10,3);
        lcd.print("|");
        delay(2500);
       }    

   if (voltFault == true) { 
       lcd.setCursor(0,1);
       lcd.print(battVolt);
       lcd.setCursor(6,1);
       lcd.print(voltageFault);   
       delay(2500);
       }

   if (tempFault == true) {
       lcd.setCursor(0,3);
       lcd.print(battTemperature);
       lcd.setCursor(6,3);
       lcd.print(temperatureFault);
       delay(2500);
       }  
   
   if (oilFault == true) {
       lcd.setCursor(11,1);
       lcd.print(oilLevelFault);
       delay(2500);
       }  

     //READ TEMPERTURE AND TURN ON FAN IF REQUIRED
      ambTemperature = readAmbTemp();
      if ((battTemperature>80.0) && (battTemperature-ambTemperature >5.0)){
         digitalWrite(fanControl,HIGH);  //TURN ON FAN
         fanOn=true;       //SET FAN FLAG  
         Serial.print("fanOn= ");
         Serial.println(fanOn);
         } 
   
   //CREATE A CURRENT FAULT TEST, FOR BOTH HIGH AND LOW CURRENT
   //SHOULD THE FAN TURN ON TO HEAT BATTERIES AS WELL AS COOL?
   //TURN OFF FAN WHEN BATTERY TEMPERATURE <= 80F
   //IF AMBIENT TEMP IS HIGH AND COOLING WILL NOT OCCUR WHAT HAPPENS?
   //ADD A FAN TACH OR ROTOR SIGNAL IN NEXT H/W VERSION?  
   //SEND POWER UP TIME, VOLTAGE, AND TEMPERATURE TO DATA LOG

  
  }   //CLOSING BRACE FOR SETUP FUNCTION                     
  
 //End of setup loop ------------------------------------------------

 void loop() {
  

   Serial.print("Voltage Fault= ");
   Serial.println(voltFault);
   Serial.print("Temp Fault= ");
   Serial.println(tempFault);
   Serial.print("Oil Level Fault= ");
   Serial.println(oilFault);
   delay(2500);
   battVolt = readBattVolt();
   Serial.print("Battery Voltage= ");
   Serial.println(battVolt);
   battCurrent = readBattAmps();
   Serial.print("Battery Current= ");
   Serial.println(battCurrent);
   readSensor(A3);
   Serial.print("Crank Battery Voltage= ");
   Serial.println(crankVolts);
   battTemperature = readBattTemp();
   Serial.print("Battery Temperature= ");
   Serial.println(battTemperature);
   ambTemperature = readAmbTemp();
   Serial.print("Ambient Temperature= ");
   Serial.println(ambTemperature);
   Serial.print("Mode=");
   Serial.println(mode); 
   runTime = readSwitch();
   Serial.print("RunTime=");
   Serial.println(runTime);
   delay(1000);
 }
