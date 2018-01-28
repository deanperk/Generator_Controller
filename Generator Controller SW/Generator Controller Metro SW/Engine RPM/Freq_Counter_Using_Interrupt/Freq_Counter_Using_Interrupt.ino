/* Freq Counter Using Interrupt
 *   Frequency counter sketch, for measuring frequencies low enough to execute an interrupt for each cycle
 *   Connect the frequency source to the INT0 pin (digital pin 2 on an Arduino Uno)
 *   REWRITE THIS  - WILL NOT WORK WITH AUNTO MANUAL SWITCH CODE USING SAME INTERRUPT
 */


// include the library code
 #include "Wire.h"
 #include "Adafruit_LiquidCrystal.h"
 Adafruit_LiquidCrystal lcd(0); // Connect via i2c, default address #0 (A0-A2 not jumpered)

volatile unsigned long firstPulseTime;
volatile unsigned long lastPulseTime;
volatile unsigned long numPulses;

void isr()
{
 unsigned long now = micros();
 if (numPulses == 1)
 {
   firstPulseTime = now;
 }
 else
 {
   lastPulseTime = now;
 }
 ++numPulses;
}

void setup()
{
 Serial.begin(9600);    // this is here so that we can print the result
 lcd.begin(20,4); // set up the LCD's number of rows and columns: 
    lcd.setCursor(2,0); // Print opening message to the LCD.
    lcd.print("Autonomous Power");
    lcd.setCursor(7,1);
    lcd.print("Systems");
    lcd.setCursor(5,3);
    lcd.print("F/W Ver 0.1");
    delay (2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Engine RPM =  ");
}

// Measure the frequency over the specified sample time in milliseconds, returning the frequency in Hz
float readFrequency(unsigned int sampleTime)
{
 numPulses = 0;                      // prime the system to start a new reading
 attachInterrupt(0, isr, RISING);    // enable the interrupt
 delay(sampleTime);
 detachInterrupt(0);
 return (numPulses < 3) ? 0 : (1000000.0 * (float)(numPulses - 2))/(float)(lastPulseTime - firstPulseTime);
}

void loop()
{
 float freq = readFrequency(1000);
 lcd.setCursor(14,0);
 lcd.print(freq);
 Serial.println(freq);
 delay(1000);
}
