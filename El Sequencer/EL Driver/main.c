/*
    EL Driver test code
	Pete Dokter, 5/20/09
	
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define EL_G 0	//PB0
#define EL_H 1	//PB1

#define EL_A 0	//PC0
#define EL_B 1	//PC1
#define EL_C 2	//PC2
#define EL_D 3	//PC3
#define EL_E 4	//PC4
#define EL_F 5	//PC5


//Define functions
//======================
void ioinit(void);      //Initializes IO
void delay_ms(uint16_t x); //General purpose delay
void delay_us(int x);

void pulse(char line, int speed);
void cycle(int speed);
void line_on(char line);//send 'A' through 'H'
void line_off(char line);//send 'A' through 'H'

//======================

static char line_on_1 = 0;
static char line_on_2 = 0;


int main (void)
{
	int x;

    ioinit(); //Setup IO pins and defaults
	
	delay_ms(5000);
	
	while(1)
	{
		for (x = 0; x < 4; x++)
		{
			pulse('A',125);
			delay_ms(100);
		}
			
		for (x = 10000; x < 25000; x+=1000)
		{
			cycle(x);
		}
		
		for (x = 25000; x < 32000; x+=200)
		{
			cycle(x);
		}
		
		for (x = 0; x < 1600; x++)
		{
			cycle(32000);
		}

		for (x = 0; x < 15; x++)
		{
			line_on('F');
			delay_ms(20);
			line_on('B');
			delay_ms(20);
			line_on('H');
			delay_ms(20);
			line_on('E');
			delay_ms(20);
			line_on('C');
			delay_ms(20);
			line_on('A');
			delay_ms(20);
			line_on('D');
			delay_ms(20);
			line_on('G');
			delay_ms(20);
			
		}
		
		line_off('D');
		line_off('G');
		
		for (x = 0; x < 4; x++)
		{
			pulse('A',250);
			delay_ms(100);
		}
		
		delay_ms(1000);
		
		
	}
	
	
}

void ioinit (void)
{
	PORTB = 0;
	DDRB = 0;
	
	PORTC = 0;
	DDRC = 0;

    TCCR2B = (1<<CS21); //Set Prescaler to 8. CS21=1

}


//General short delays
void delay_us(int x)
{
	int y, z, a;
	
	y = x/256;
	z = x - y * 256;
	
	for (a = 0; a < y; a++)
	{
		TIFR2 |= 0x01;//Clear any interrupt flags on Timer2
		
		TCNT2 = 0; //256 - 125 = 131 : Preload timer 2 for x clicks. Should be 1us per click
	
		while(!(TIFR2 & 0x01));
		
	}
	
	TIFR2 |= 0x01;//Clear any interrupt flags on Timer2
	
	TCNT2 = 256-z; //256 - 125 = 131 : Preload timer 2 for x clicks. Should be 1us per click

	while(!(TIFR2 & 0x01));
	
}

//General short delays
void delay_ms(uint16_t x)
{
	for (; x > 0 ; x--)
    {
        delay_us(250);
        delay_us(250);
        delay_us(250);
        delay_us(250);
    }
	
}


void pulse(char line, int speed)
{
	int x;
	
	for (x = 0; x < 10000; x+=speed)
	{
		line_on(line);
		delay_us(x + 100);
		line_off(line);
		delay_us(10000 - x);
	}

	for (x = 0; x < 10000; x+=speed)
	{
		line_on(line);
		delay_us(10100 - x);
		line_off(line);
		delay_us(x + 100);
	}
	
}


void line_on(char line)//send 'A' through 'H'
{
	char temp;
	
	if (line_on_2 != 0) line_off(line_on_2);//can't have more than one line on at a time
	
	//keep track of what's on and in what sequence
	line_on_2 = line_on_1;
	line_on_1 = line;

	temp = line - 65;
	
	if (temp < 6) DDRC |= (1<<temp);
	else DDRB |= (1<<(temp-6));

}


void line_off(char line)//send 'A' through 'H'
{
	char temp;
	
	if (line == line_on_2) line_on_2 = 0;
	else if (line == line_on_1)
	{
		line_on_1 = line_on_2;
		line_on_2 = 0;
	}
	
	temp = line - 65;
	
	if (temp < 6) DDRC &= ~(1<<temp);
	else DDRB &= ~(1<<(temp-6));

}

void cycle(int speed)
{
	int x;
	
	if (speed > 32000) speed = 32000;
	
	for (x = 65; x < 72; x++)
	{
		line_on(x);
		delay_us(32100 - speed);
	}
	
	for (x = 72; x >= 65; x--)
	{
		line_on(x);
		delay_us(32100 - speed);
	}
	
	line_off('B');
	delay_us(32100 - speed);
	line_off('A');
}



