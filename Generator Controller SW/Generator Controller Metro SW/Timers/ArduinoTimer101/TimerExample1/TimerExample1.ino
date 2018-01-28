/* Arduino 101: timer and interrupts
 1: Timer1 compare match interrupt example 
 more infos: http://www.letmakerobots.com/node/28278
 created by RobotFreak 
 */

long unsigned int countHour=0;
int x=0;

void setup()
{ 
  Serial.begin(9600);
 
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 62500;            // compare match register 16MHz/256/1Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
 countHour = countHour +1;
}

void loop()
{
  x= (countHour % 60);
  Serial.println(x);
  if (x == 0) {
  Serial.println("1 minute");
  }
}


