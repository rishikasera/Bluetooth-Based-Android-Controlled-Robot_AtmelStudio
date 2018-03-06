/*
 * RoBlue.c
 *
 * Created: 17/02/2018 1:30:14 PM
 *  Author: home pc
 */ 
//update ISR

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void SetBit(char);
void ResetBit(char);

void BlueLed();
void TestFun();
void reset();

char buffer[100];
volatile int ind = 0;
char verified = 0;

volatile  int rf = 0;
volatile  int rb = 0;
volatile  int lf = 0;
volatile  int lb = 0;

void uart_transmit (unsigned char data);
void send();
void Robot(char);

ISR(USART_RXC_vect)
{
	char ReceivedByte = 'a';
	ReceivedByte = UDR; // Fetch the received byte value into the variable "ByteReceived"
	
	if(verified == 0)
	{
		buffer[ind] = ReceivedByte;
		ind++;
		if(ind >= 7)
		{
			cli();
			if(buffer[0] == 's' && buffer[1] == 'u' && buffer[2] == 'c' && buffer[3] == 'c' && buffer[4] == 'e' && buffer[5] == 's' && buffer[6] == 's')
			{
				verified = 1;
				BlueLed();
				_delay_ms(1000);
				sei();
				ind = 0;
			}
			else
			{
				reset();
			}
		}
	}
	else{
		Robot(ReceivedByte);
	}
	//UDR = ReceivedByte; // Echo back the received byte back to the computer
}

// 1110 E
// 1101 D

int main (void)
{
	DDRC = 0xFF;
	PORTC = 0xFF;
	_delay_ms(400);
	PORTC = 0xEF;
	_delay_ms(400);
	PORTC = 0xFF;
	_delay_ms(400);
	PORTC = 0xEF;
	_delay_ms(400);
	PORTC = 0xFF;
	_delay_ms(400);
	PORTC = 0xEF;
	_delay_ms(400);
	PORTC = 0xFF;
	
	UCSRB = (1 << RXEN) | (1 << TXEN);   // Turn on the transmission and reception circuitry
	UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); // Use 8-bit character sizes

	UBRRH = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRRL = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register

	UCSRB |= (1 << RXCIE); // Enable the USART Recieve Complete interrupt (USART_RXC)
	sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
	
	//send();
	//PORTC = 0x1F;
	
	
	
	for (;;) // Loop forever
	{
		//send();
		
		// Do nothing - echoing is handled by the ISR instead of in the main loop
	}
}

// function to send data - NOT REQUIRED FOR THIS PROGRAM IMPLEMENTATION
void uart_transmit (unsigned char data)
{
	while (!( UCSRA & (1<<UDRE)));            // wait while register is free
	UDR = data;                             // load data in the register
}


void send()
{
	uart_transmit('A');
	_delay_ms(10);
	uart_transmit('T');
	_delay_ms(10);
	uart_transmit(0x0D);
	_delay_ms(10);
	uart_transmit(0x0A);
	_delay_ms(1000);
}

void BlueLed()
{
	PORTC = 0xDF;
	_delay_ms(400);
	PORTC = 0xFF;
	_delay_ms(400);
	PORTC = 0xDF;
	_delay_ms(400);
	PORTC = 0xFF;
	_delay_ms(400);
	PORTC = 0xDF;
	_delay_ms(400);
	PORTC = 0xFF;
}


void reset()
{
	while(1)
	{
		SetBit(5);
		SetBit(6);
		_delay_ms(400);
		ResetBit(5);
		ResetBit(6);
		_delay_ms(400);
	}
	
}

void SetBit(char b)
{
	PORTC = PORTC | (1<<(b-1));
}
void ResetBit(char b)
{
	PORTC = PORTC & ~(1<<(b-1));
}

/*
F   R S R S  
B   S R S R
R   R S S R
L   S R R S
S   S S S S

B   S S
D   S S
F         S S
H         S S

A   S R
C   R S
E         S R
G         R S
*/
void Robot(char cmd)
{	



	if(cmd == 'A' && rb == 0)
	{
		rf = 1;
		SetBit(1);
		ResetBit(2);
	}
	else if(cmd == 'B')
	{
		SetBit(2);
		rf = 0;
	}
	
	else if(cmd == 'C' && rf == 0)
	{
		rb = 1;
		ResetBit(1);
		SetBit(2);
	}
	else if(cmd == 'D')
	{
		SetBit(1);
		rb = 0;
	}
	
	else if(cmd == 'E' && lb == 0)
	{
		lf = 1;
		SetBit(3);
		ResetBit(4);
	}
	else if(cmd == 'F')
	{
		SetBit(4);
		lf = 0;
	}
	
	else if(cmd == 'G' && lf == 0)
	{
		lb = 1 ;
		ResetBit(3);
		SetBit(4);
	}
	else if(cmd == 'H' )
	{
		SetBit(3);
		lb = 0;
	}
	
}


void _Robot(char cmd)
{
	if(cmd == 'f' || cmd == 'F')
	{
		ResetBit(1);
		SetBit(2);
		ResetBit(3);
		SetBit(4);
	}
	else if(cmd == 'b' || cmd == 'B')
	{
		ResetBit(2);
		SetBit(1);
		ResetBit(4);
		SetBit(3);
	}
	else if(cmd == 'l' || cmd == 'L')
	{
		SetBit(1);
		ResetBit(2);
		ResetBit(3);
		SetBit(4);
		_delay_ms(400);
		SetBit(3);
		SetBit(2);
	}
	else if(cmd == 'r' || cmd == 'R')
	{
		ResetBit(1);
		SetBit(2);
		SetBit(3);
		ResetBit(4);
		_delay_ms(400);
		SetBit(1);
		SetBit(4);
	}
	else if(cmd == 's' || cmd == 'S')
	{
		SetBit(1);
		SetBit(2);
		SetBit(3);
		SetBit(4);
	}
}
