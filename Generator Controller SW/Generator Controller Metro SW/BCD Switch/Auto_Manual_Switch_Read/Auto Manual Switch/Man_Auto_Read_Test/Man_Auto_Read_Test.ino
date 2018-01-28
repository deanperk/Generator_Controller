/* 

Man Auto Test Program
Program reads the integer value of a 3 position toggle switch and returns a
value for the manual or auto mode 
manual switchpole on D2, automatic switch pole on D3

*/

const int manual=2;
const int automatic=3;
int mode=0;
int total=0;


 void setup()
{
 Serial.begin(9600);
 delay(500);
 pinMode(manual, INPUT);
 digitalWrite(manual,HIGH);
 pinMode(automatic, INPUT);
 digitalWrite(automatic,HIGH);
}

// Function Definition
int readSwitch()
{
 int total=0;
 Serial.print ("function total A =  ");
 Serial.println (total);
 
 if (digitalRead(manual)==HIGH) { total+=1; }
 if (digitalRead(automatic)== HIGH) { total+=2; }
  
 Serial.print ("function total B =  ");
 Serial.println (total);
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
 return mode;
 }
 


void loop()
{
 readSwitch();
 Serial.println (mode);
 delay(2000);
}

