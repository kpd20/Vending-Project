/*
 * Motor Microcontroller.cpp
 *
 * Created: 7/18/2019 11:41:11 AM
 * Author : Kyle Draper, Dr. William Clark (delay functions)
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

/* GLOBAL VARIABLES */
volatile int mult = 50, t = 1; // delay function selections --> 250msec
int direction = 0; // for motor angular position
volatile char sensor = 0; // value read from analog sensor (0-1023)

/* FUNCTION DECLARATIONS */
void stepper_1(void);
void stepper_2(void);
void stepper_3(void);
void stepper_4(void);
void wait(volatile int multiple, volatile char time_choice);
void delay_T_msec_timer0(char choice);
void initialize_usart(void);

int main(void)
{
    // PIN SETUP
		DDRB = 0xFF;
		DDRD = 0xFF;
		DDRC =
			(0<<DDC5)|(1<<DDC4)|(1<<DDC3)|(1<<DDC2)|(1<<DDC1)|(1<<DDC0);
	
		PORTB = 0x00; // set to 0V
		PORTC = 0x00; // set to 0V
		PORTD = 0x00; // set to 0V
	
	// MOTOR SETUP
		#define RED_1		_BV(PD1)
		#define BLUE_1		_BV(PD2)
		#define GREEN_1		_BV(PD3)
		#define BLACK_1		_BV(PD4)
		
		#define RED_2		_BV(PD5)
		#define BLUE_2		_BV(PD6)
		#define GREEN_2		_BV(PD7)
		#define BLACK_2		_BV(PB0)
		
		#define RED_3		_BV(PC4)
		#define BLUE_3		_BV(PC3)
		#define GREEN_3		_BV(PC2)
		#define BLACK_3		_BV(PC1)
		
		#define RED_4		_BV(PB4)
		#define BLUE_4		_BV(PB3)
		#define GREEN_4		_BV(PB2)
		#define BLACK_4		_BV(PB1)
	
	// ADC SETUP
		PRR = 0x00; // clear Power Reduction ADC bit (0)
		ADCSRA = // set ADC Enable bit, set ADC prescaler to 128
		(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
		ADMUX = // ADC5 MUX Bits 3..0 = 0101
		// left justify (ADLAR = 1) --> ADCH justified
		// AVCC external reference (REF$1 = 0) & (REF$0 = 1)
		(0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(1<<MUX2)|(0<<MUX1)|(1<<MUX0);
	
	
	/* PROGRAM CODE */
    while (1)
    {
		// VARIABLES
		unsigned char select = 0;
		
		// initialize USART with desired parameters
		initialize_usart();
		
		// read analog input
		ADCSRA |= (1<<ADSC); // start analog conversion
		while ((ADCSRA & (1<<ADIF)) == 0); // wait for conversion to finish
		
		sensor = ADCH; // keep high byte of sensor result
		
		// evaluate sensor results
		
		
		// read in data
		while(! (UCSR0A & (1<<RXC0))); // wait until new data arrives
		select = UDR0;
		
		// select motor
		switch(select){
			case 49:
				stepper_1();
					break;
			case 50:
				stepper_2();
					break;
			case 51:
				stepper_3();
					break;
			case 52:
				stepper_4();
					break;
		}
		
		
		
	}
	
	return(0);
} // END main


// ================
// INTERRUPTS
// ================


// ================
// FUNCTIONS
// ================
void stepper_1(void) {
	PORTD = 0x00;
	for (int i=0; i<100; i++) {
		PORTD = BLUE_1;
		wait(mult,t);
		PORTD = GREEN_1;
		wait(mult,t);
		PORTD = RED_1;
		wait(mult,t);
		PORTD = BLACK_1;
		wait(mult,t);
	}
	PORTD = 0x00;
	wait(5000,t);
}

void stepper_2(void) {
	PORTD = 0x00;
	PORTB = 0x00;
	for (int i=0; i<100; i++) {
		PORTD = BLUE_2;
		wait(mult,t);
		PORTD = GREEN_2;
		wait(mult,t);
		PORTB = RED_2;
		PORTB = 0x00;
		wait(mult,t);
		PORTD = BLACK_2;
		wait(mult,t);
	}
	PORTD = 0x00;
	PORTB = 0x00;
	wait(5000,t);
}

void stepper_3(void) {
	PORTC = 0x00;
	for (int i=0; i<100; i++) {
		PORTC = BLUE_3;
		wait(mult,t);
		PORTC = GREEN_3;
		wait(mult,t);
		PORTC = RED_3;
		wait(mult,t);
		PORTC = BLACK_3;
		wait(mult,t);
	}
	PORTC = 0x00;
	wait(5000,t);
}

void stepper_4(void) {
	PORTD = 0x00;
	for (int i=0; i<100; i++) {
		PORTB = BLUE_4;
		wait(mult,t);
		PORTB = GREEN_4;
		wait(mult,t);
		PORTB = RED_4;
		wait(mult,t);
		PORTB = BLACK_4;
		wait(mult,t);
	}
	PORTD = 0x00;
	wait(5000,t);
}

void initialize_usart(void) {
	UCSR0B = (1<<RXEN0); // enable serial transmission
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // asynchronous mode; 8-bit; no parity; 1-stop bit
	UBRR0L = 0x67; // 9600 baud for FOsc = 16MHz
}

void wait(volatile int multiple, volatile char time_choice) {
	/* This subroutine calls others to create a delay.
		 Total delay = multiple*T, where T is in msec and is the delay created by the called function.

		Inputs: multiple = number of multiples to delay, where multiple is the number of times an actual delay loop is called.
		Outputs: None
	*/

	while (multiple > 0) {
		delay_T_msec_timer0(time_choice); 
		multiple--;
	}
} // end wait()
	
	
void delay_T_msec_timer0(char choice) {
    //*** delay T ms **
    /* This subroutine creates a delay of T msec using TIMER0 with prescaler on clock, where, for a 16MHz clock:
    		T = .0156 msec for no prescaler and count of 250 (preload counter with 5)
    		T = 0.125 msec for prescaler set to 8 and count of 250 (preload counter with 5)
    		T = 1 msec for prescaler set to 64 and count of 250 (preload counter with 5)
    		T = 4 msec for prescaler set to 256 and count of 250 (preload counter with 5)
    		T = 16 msec for prescaler set to 1,024 and count of 250 (preload counter with 5)

			Inputs: None
			Outputs: None
	*/
	
	TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	TCNT0 = 5;  // preload load TIMER0  (count must reach 255-5 = 250)

	switch ( choice ) { // choose prescaler
		case 1:
			TCCR0B = 1<<CS11;			//TCCR0B = 0x02; // Start TIMER0, Normal mode, crystal clock, prescaler = 8
		break;

		case 2:
			TCCR0B =  1<<CS11 | 1<<CS10;	//TCCR0B = 0x03;  // Start TIMER0, Normal mode, crystal clock, prescaler = 64
		break;

		case 3:
			TCCR0B = 1<<CS12;				//TCCR0B = 0x04; // Start TIMER0, Normal mode, crystal clock, prescaler = 256
		break; 

		case 4:
			TCCR0B = 1<<CS12 | 1<<CS10;	//TCCR0B = 0x05; // Start TIMER0, Normal mode, crystal clock, prescaler = 1024
		break;

		default:
			TCCR0B = 1<<CS10;				//TCCR0B = 0x01; Start TIMER0, Normal mode, crystal clock, no prescaler
		break;
	}
	
	while ((TIFR0 & (0x1<<TOV0)) == 0); // wait for TOV0 to roll over and trigger TOV0 interrupt 

	TCCR0B = 0x00; // Stop TIMER0
	TIFR0 = 0x1<<TOV0; // Clear TOV0 (note that this is an odd bit in that it is cleared
						// by writing a 1 to it)
} // end delay_T_msec_timer0()