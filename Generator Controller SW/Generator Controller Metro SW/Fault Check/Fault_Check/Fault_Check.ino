   //This function should be rewritten to eliminate the unnecessary execution of "if" statements
   
   void checkFault() {
   //DETERMINE IF OIL LEVEL FAULT EXISTS
   if (digitalRead(oilSwitch) == LOW){
       oilFault = true;
       }
      else {
        oilFault = false;
      }   

  //DETERMINE IF A BATTERY TEMPERATURE FAULT EXISTS
      battTemperature = readBattTemp();
   if (battTemperature >=100.0) {
       tempFault = true;
       }
       else {
        tempFault = false;
      }   
 
   //DETERMINE IF A BATTERY VOLTAGE FAULT EXISTS
   battVolt = readBattVolt();
   if (battVolt <=22.0) {
      voltFault = true;
      }
      else {
        voltFault = false;
      }   

   //DETERMINE IF A CURRENT FAULT EXISTS
   if (engineRun == true){
      battCurrent = readBattAmps();
      if ((battCurrent >90.0) || (battCurrent <30.0)){
          currentFault = true;}
      }
      else { 
        currentFault = false;
        } 
   
   //SET UP FAULT DISPLAY 
   if ((oilFault == true) || (tempFault == true) || (voltFault == true) || (currentFault == true)){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(sysBattFault); 
        lcd.setCursor(10,1);       
        lcd.print("|");            
        lcd.setCursor(10,2);       
        lcd.print("|");           
        lcd.setCursor(10,3);       
        lcd.print("|");
        }
   
   if (tempFault == true) {
       lcd.setCursor(0,3);
       lcd.print(battTemperature);
       lcd.setCursor(6,3);
       lcd.print(temperatureFault);
       digitalWrite(faultAlarm,HIGH);  //set Fault Alarm contact closure
       digitalWrite(startEnable,LOW);  //prevent engine from running  
       }  
   
   if (oilFault == true) {
       lcd.setCursor(11,1);
       lcd.print(oilLevelFault);
       digitalWrite(faultAlarm,HIGH);  //set Fault Alarm contact closure
       digitalWrite(startEnable,LOW);  //prevent engine from running  
       }  

   if (currentFault == true) { 
       lcd.setCursor(0,2);
       lcd.print(battCurrent);
       lcd.setCursor(6,2);
       lcd.print(ampFault);   
       digitalWrite(faultAlarm,HIGH);  //set Fault Alarm contact closure
       }

   if (voltFault == true) { 
       lcd.setCursor(0,1);
       lcd.print(battVolt);
       lcd.setCursor(6,1);
       lcd.print(voltageFault);   
       digitalWrite(faultAlarm,HIGH);  //set Fault Alarm contact closure
       }
   
       }//closing brace for checkFault function