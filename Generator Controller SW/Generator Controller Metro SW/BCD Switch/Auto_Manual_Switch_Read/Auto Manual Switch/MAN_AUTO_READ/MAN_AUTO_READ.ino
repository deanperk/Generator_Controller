//Test MAN/AUTO Switch Read

// Variables
 
 // DOES NOT WORK AS WRITTEN LOOK FOR ERRORS
 
 const int manual = 2;
 const int automatic = 3;
 int manAuto = 0;

// Read Man/Auto Switch function
int readSwitch()
{
 int manAuto=0;

 if (digitalRead(manual)==HIGH) { manAuto +=1; }
 if (digitalRead (automatic)==HIGH) { manAuto +=2; }

/* Set manual automatic status based on Auto?Man switch status
 switch (manAuto) {
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
      } */
 return manAuto;

}

void setup() {
 Serial.begin(9600);
 delay(500);
 pinMode(manual, INPUT);
 digitalWrite(manual,HIGH);
 pinMode(automatic, INPUT);
 digitalWrite(automatic,HIGH);
 }

void loop() {
readSwitch();
Serial.println(manAuto);

}
