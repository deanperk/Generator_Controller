/* Interrupt test code for Auto/Man switch
 * uses hardware interrupts on pin 2 (Interrout 0) and pin 3 (Interrupt 1) to set 
 * Auto/Man mode. void loop() uses a simple blink program 
 * to demonstrate some activity in the main loop
 * When using a variable that will get passed pack to teh main loop, declare it as VOLATILE
 */
const int LED1 = 10;
const int LED2 = 9;
const int LED3 = 8;
const int LED4 = 7;

const int ledPin = 4;
// const int MAN = 2;
// const int AUTO = 3;
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000; 
const int manual=2;
const int automatic=3;
volatile int mode=0;
volatile int total=0;

// Manual Mode Interrupt Service Routine (ISR)
void switchPressed1 ()
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


// Auto Mode Interrupt Service Routine (ISR)
void switchPressed2 ()
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
}  // end of switchPressed2


void setup ()
{
  pinMode(manual, INPUT);
  digitalWrite(manual,HIGH);
  pinMode(automatic, INPUT);
  digitalWrite(automatic,HIGH);
  pinMode (LED1, OUTPUT);  // so we can update the LED
  pinMode (LED2, OUTPUT);
  pinMode (LED3, OUTPUT);
  pinMode (LED4, OUTPUT);
  pinMode (ledPin, OUTPUT);
  attachInterrupt (0, switchPressed1, CHANGE);  // attach interrupt handler
  attachInterrupt (1, switchPressed2, CHANGE);  // attach interrupt handler
  

 }  // end of setup



void loop ()
{
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


}









