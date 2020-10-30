#include <avr/io.h>
#include <avr/interrupt.h>
  
// global variable to count the number of overflows
volatile uint16_t tot_overflow;
  
// initialize timer, interrupt and variable
void timer0_init()
{
  // Timer/Counter 0 initialization
  // Clock source: System Clock
  TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
  
  // Clock value: 37.5 kHz
  // Mode: Normal top=0xFF
  TCCR0B=(0<<WGM02) | (1<<CS02) | (0<<CS01) | (0<<CS00); //9600000/256 = 37.5KHZ
  
  // initialize counter
  TCNT0 = 0;
  // OC0A output: Disconnected
  // OC0B output: Disconnected
  OCR0A=0x00;
  OCR0B=0x00;

  // enable overflow interrupt
  TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (1<<TOIE0);

  // External Interrupt(s) initialization
  // INT0: Off
  // Interrupt on any change on pins PCINT0-5: Off
  GIMSK=(0<<INT0) | (0<<PCIE);
  MCUCR=(0<<ISC01) | (0<<ISC00);

  // ADC initialization
  // ADC disabled
  ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);
  
  // enable global interrupts
  sei();
  
  // initialize overflow counter variable
  tot_overflow = 0;
}
  
// TIMER0 overflow interrupt service routine
// called whenever TCNT0 overflows
ISR(TIM0_OVF_vect)
{
    // keep a track of number of overflows
    tot_overflow++;
}
  
int main(void)
{
  // clear all interrupts
  cli();
  // Crystal Oscillator division factor: 1
  //#pragma optsize-
  CLKPR=(1<<CLKPCE);
  CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);

  // Input/Output Ports initialization
  // Port B initialization
  // Function: Bit5=In Bit4=Out Bit3=Out Bit2=In Bit1=Out Bit0=In
  DDRB=(0<<DDB5) | (1<<DDB4) | (1<<DDB3) | (0<<DDB2) | (1<<DDB1) | (0<<DDB0);
// State: Bit5=T Bit4=0 Bit3=T Bit2=T Bit1=0 Bit0=T
  PORTB=(0<<PB5) | (0<<PB4) | (0<<PB3) | (0<<PB2) | (0<<PB1) | (0<<PB0);
  
  // initialize timer
  timer0_init();
  
    // loop forever
    while(1)
    {
        // check if no. of overflows = 8789
        if (tot_overflow >= (8789))//240*60))  // NOTE: '>=' is used
        {
            // check if the timer count reaches 72
            if (TCNT0 >= 72)
            {
		PORTB ^= (1<<PB1) | (1<<PB3) | (1<<PB4);  // toggles the Relay2(IN2),toggles the Relay1(IN1), toggles the led's 
		TCNT0 = 0;            // reset counter
                tot_overflow = 0;     // reset overflow counter
            }
        }
    }
}
//#define F_CPU 1000000  // 1 MHz
/*#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB |= _BV(PB3);  // PB3 as output
	DDRB |= _BV(PB1);
	DDRB |= _BV(PB4);
    for (;;) {
	PORTB ^= (1<<PB1) | (1<<PB3) | (1<<PB4);
	_delay_ms(5000);     // 1 min
    }
}*/
