//Test BCD Runtime Switch code

// Variables
 const int q1 = 11;
 const int q2 = 12;
 const int q4 = 13;

// Read BCD Switch function
int readSwitch()
{
 int bcdTotal=0;
 int runTime=0;
 if (digitalRead(q1)==HIGH) { bcdTotal+=1; }
 if (digitalRead(q2)==HIGH) { bcdTotal+=2; }
 if (digitalRead(q4)==HIGH) { bcdTotal+=4; }
 // Set manual run time based on BCD switch input
 switch (bcdTotal) {
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

void setup() {
 Serial.begin(9600);
 delay(500);
 // pinMode(q1, INPUT); // thumbwheel '1'
 // digitalWrite(q1,HIGH);
 // pinMode(q2, INPUT); // thumbwheel '2'
 // digitalWrite(q2,HIGH);
 pinMode(q4,INPUT); // thumbwheel '4'
 digitalWrite(q4,HIGH);
}

void loop() {
readSwitch();
Serial.println(runTime);

}
