
unsigned long countHour=0;
unsigned long x=0;


void setup(){

noInterrupts();      // disable all interrupts

//set timer1 interrupt at 1Hz
  TCCR1A = 0;        // set entire TCCR1A register to 0
  TCCR1B = 0;        // same for TCCR1B
  TCNT1  = 0;        //initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;     // = (16mHz) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

interrupts();        // enable all interrupts

ISR(TIMER1_COMPA_vect)      //timer compare interrupt service routine
{
 countHour += 1;            //increment countHour on each interrupt
}


}//end setup

void loop()
{
  x=(countHour % 60); //test for 1 minute
  Serial.print("countHour = ");
  Serial.print(countHour);
  Serial.print("   x = ");
  Serial.println(x);
  if (x == 0) {
  Serial.println("1 minute");
  }
}