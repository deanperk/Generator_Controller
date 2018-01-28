/* 

Man Auto Read Test 2
Program reads the integer value of a 3 position toggle switch and returns a
value for the manual or auto mode 
manual switchpole on D2, automatic switch pole on D3

*/

const int manual=2; //D2
const int automatic=3; //D3
int mode=0;
int total=0;


 void setup()
{
 Serial.begin(9600);
 delay(500);
 pinMode(manual,INPUT);
 digitalWrite(manual,HIGH);
 pinMode(automatic, INPUT);
 digitalWrite(automatic,HIGH);
}

// Function Definition
int readAutoMan()
{
 int total=0;
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
 return mode;
 }
 


void loop()
{
 readAutoMan();
 if (mode == 0){
  Serial.println("Fault");
 }
 
 if (mode == 1){
  Serial.println("Automatic");
 }
 if (mode == 2){
  Serial.println("Manual");
 }
 if (mode == 3){
  Serial.println("Off");
 }

 delay(2000);
}

