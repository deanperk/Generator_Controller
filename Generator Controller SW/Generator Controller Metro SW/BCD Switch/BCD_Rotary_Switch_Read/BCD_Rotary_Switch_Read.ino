/* 
BCD Rotary Switch Read
Program reads the integer value of an Octal BCD encoded rotary switch and returns a
value in hours for the manual mode run time  
BCD switchcode taken from Lesson 40 on Tronixstuff website
To fuction correctly on an Arduino with a D13 LED, the LED must be
removed from the Arduino board

*/
const int q1=11;
const int q2=12;
const int q4=13;
int runTime=0;


 void setup()
{
 Serial.begin(9600);
 delay(500);
 pinMode(q1, INPUT); // thumbwheel '1'
 digitalWrite(q1,HIGH);
 pinMode(q2, INPUT); // thumbwheel '2'
 digitalWrite(q2,HIGH);
 pinMode(q4,INPUT); // thumbwheel '4'
 digitalWrite(q4,HIGH);
}

// Function Definition
int readSwitch()
{
 int total=0;
// int runTime=0;
 if (digitalRead(q1)==HIGH) { total+=1; }
 if (digitalRead(q2)==HIGH) { total+=2; }
 if (digitalRead(q4)==HIGH) { total+=4; }
 // Set manual run time based on BCD switch input
 switch (total){
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

// End function definition

void loop()
{
 readSwitch();
 Serial.println(runTime);
}

