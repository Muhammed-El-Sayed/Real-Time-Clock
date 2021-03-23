//F_CPU 1000000

#include<avr/io.h>
#include<avr/interrupt.h>
#include"util/delay.h"

unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;


void Timer1_CTC_init(void)
{
	cli();                                           //disable global interrupt
	TCCR1A |= (1<<FOC1A) | (1<<FOC1B);              //Non-PWM so,we set FOC1A/B
	TCCR1B |= (1<<WGM12) | (1<<CS12) | (1<<CS10);  //CTC mode, 1024 Prescaler:T(timer)=1ms
	TCNT1 =0;              //Initial value of Timer1 = 0
	OCR1A = 1000;         //Top & Compare value = 1000
	sei();               //set Global interrupt enable
	TIMSK = 1<<OCIE1A;  //set Compare match interrupt module enable
}

void INT0_init(void)
{
	cli();               //disable global interrupt
	DDRD &= ~(1<<PD2);  //Set PD2 input
	PORTD |= 1<<PD2;   //Activate PD2 internal pull up resistor
	                  //event on falling edge
	MCUCR |= 1<<ISC01;
	MCUCR &= ~(1<<ISC00);

	sei();            //enable global interrupt
	GICR |= 1<<INT0; //enable module interrupt
}

void INT1_init(void)
{
	cli();                                //disable global interrupt
	DDRD &= ~(1<<PD3);                   //Set PD3 input
	MCUCR |= (1<<ISC11) | (1<<ISC10);   //event on rising edge

	sei();                                //enable global interrupt
	GICR  |= 1<<INT1;                    //enable module interrupt
}

void INT2_init(void)
{
	cli();                     //disable global interrupt
	DDRB &= ~(1<<PB2);        //set Pin PB2 input
    PORTB |= 1<<PB2;         //Activate PB2 internal pull up resistor
	MCUCSR &= ~(1<<ISC2);   //event on falling edge

    sei();                     //enable global interrupt
	GICR  |= 1<<INT2;         //enable module interrupt
}
int main(void)
{
	DDRA |= 0X3F;                         //set PINS(PA0-PA5) output (enables pins for 7-segments)
	DDRC |= 0X0F;                        //set PINS(PC0-PC3) outputs (connected to BCD 7447)

	Timer1_CTC_init();             //Calling Timer to start counting
	INT0_init();                  //Initialize INT0
	INT1_init();                 //Initialize INT1
	INT2_init();                //Initialize INT2

	while(1)
	{
		//SEC Units
		PORTA = (PORTA & 0XC0) | 0X01; //enable SEC Units only
		PORTC = (PORTC & 0XF0) | ((seconds%10) & (0X0F)) ; //display SEC Units
		_delay_ms(5); //Refresh rate


		//SEC tenth
		PORTA = (PORTA & 0XC0) | 0X02; //enable SEC tenth only
		PORTC = (PORTC & 0XF0) | ( ((seconds/10)%10) & (0X0F)) ; //display SEC tenth
		_delay_ms(5); //Refresh rate

		//MIN Units
		PORTA = (PORTA & 0XC0) | 0X04; //enable MIN Units only
		PORTC = (PORTC & 0XF0) | ( (minutes%10) & (0X0F) ) ; //display MIN Units
		_delay_ms(5); //Refresh rate


		//MIN tenth
		PORTA = (PORTA & 0XC0) | 0X08; //enable MIN tenth only
		PORTC = (PORTC & 0XF0) | ( ((minutes/10)%10) & (0X0F)) ; //display MIN tenth
		_delay_ms(5); //Refresh rate


		//HR  Units
		PORTA = (PORTA & 0XC0) | 0X10; //enable HR Units only
		PORTC = (PORTC & 0XF0) | ( (hours%10) & (0X0F) ) ; //display HR Units
		_delay_ms(5); //Refresh rate


		//HR  tenth
		PORTA = (PORTA & 0XC0) | 0X20; //enable HR tenth only
		PORTC = (PORTC & 0XF0) |  ( ((hours/10)%10)& (0X0F)) ; //display HR tenth
		_delay_ms(5); //Refresh rate


	}
}

ISR(TIMER1_COMPA_vect)
{
	//Modify the counter each 1 sec.

	seconds = seconds + 1;
	if(seconds == 60)
	{
		seconds = seconds - 60;
		minutes= minutes + 1;
	}
	if(minutes == 60)
	{
		minutes = minutes - 60;
		hours= hours + 1;
	}

}

ISR(INT0_vect){

	//Reset Counter
	seconds =0; minutes=0; hours=0;
	TCNT1=0;

}

ISR(INT1_vect){

	//stop clock to Pause
	TCCR1B &= ~(1<<CS12) & ~(1<<CS10);
}

ISR(INT2_vect){

	//Resume by returning the clock
	TCCR1B |= 1<<CS12 | 1<<CS10;
}




