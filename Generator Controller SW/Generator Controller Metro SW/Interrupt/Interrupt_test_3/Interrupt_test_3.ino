/* Interrupt test 4
 * Interrupt test code for Auto/Man switch
 * uses hardware interrupts on pin 2 (Interrout 0) and pin 3 (Interrupt 1) to set 
 * Auto/Man mode. void loop()lights an LED
 * to specify operating mode
 * When using a variable that will get passed pack to the main loop, declare it as VOLATILE
 * 
 * CAN THE SWITCH STATEMENT BE ELIMINATED AND JUST USE THE VARIABLE MODE?
 */
const int LED1 = 10;
const int LED2 = 9;
const int LED3 = 8;
const int LED4 = 7;
const int manual=2;
const int automatic=3;
volatile int mode=3; // set to OFF state for power up
volatile int total=0;

// Manual/Auto Mode Interrupt Service Routine (ISR)
void switchPressed1()
{
 // volatile int mode=0;
 // Serial.println("interupt");
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

void setup ()
{
  Serial.begin(9600);
  pinMode(manual, INPUT);
  digitalWrite(manual,HIGH);
  pinMode(automatic, INPUT);
  digitalWrite(automatic,HIGH);
  pinMode (LED1, OUTPUT);  // so we can update the LED
  pinMode (LED2, OUTPUT);
  pinMode (LED3, OUTPUT);
  pinMode (LED4, OUTPUT);
  attachInterrupt (0, switchPressed1, CHANGE);  // attach interrupt handler
  attachInterrupt (1, switchPressed1, CHANGE);  // attach interrupt handler
  // Serial.print(total);
  // Serial.println(mode);
  //Serial.println("end of setup");
 }  // end of setup



void loop ()
{
  // Serial.print(total);
 //  Serial.println(mode);

if (mode == 0) 
{
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW); 
}

if (mode == 1) 
{
  digitalWrite(LED2, HIGH);
  digitalWrite(LED1, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW); 
}

if (mode == 2) 
{
  digitalWrite(LED3, HIGH);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED4, LOW); 
}

if (mode == 3) 
{
  digitalWrite(LED4, HIGH);
  digitalWrite(LED1, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED2, LOW); 
}
// delay(2000);
}









