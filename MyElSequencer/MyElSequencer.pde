/*
    EL Driver test code
	Pete Dokter, 5/20/09
	
*/



#define EL_A 2
#define EL_B 3
#define EL_C 4
#define EL_D 5
#define EL_E 6
#define EL_F 7
#define EL_G 8
#define EL_H 9
#define EL_MAX 10


//Define functions
//======================

void pulse(char line, int speed);
void cycle(int speed);
void line_on(char line);//send EL_A through EL_H
void line_off(char line);//send EL_A through EL_H

//======================

static char line_on_1 = 0;
static char line_on_2 = 0;


#if 1
void loop (void)
{
	int del;
	char choice;
	boolean LED = false;

	del = random(300,800);
	delay(del);

	choice = random(4);
	switch (choice)
	{
		case 0:
			Lightning(EL_A, EL_B);
			break;
		case 1:
			Lightning(EL_C, EL_D);
			break;
		case 2:
			Lightning(EL_E, EL_F);
			break;
		case 3:
			Lightning(EL_G, EL_H);
			break;
	}

	if(LED)
	{
		LED = false;
		digitalWrite(10, LOW);
	}
	else
	{
		LED = true;
		digitalWrite(10, HIGH);
	}
}

void Lightning(char white, char other)
{
	line_on(white);
	delay(20);
	line_on(other);
	delay(20);
	line_off(other);
	delay(20);
	line_off(white);

	delay(80);
	line_on(white);
	delay(20);
	line_off(white);

}



#else
void loop (void)
{
	int x;

	for (x = 0; x < 4; x++)
	{
		pulse(EL_A,125);
		delay(100);
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
		line_on(EL_F);
		delay(20);
		line_on(EL_B);
		delay(20);
		line_on(EL_H);
		delay(20);
		line_on(EL_E);
		delay(20);
		line_on(EL_C);
		delay(20);
		line_on(EL_A);
		delay(20);
		line_on(EL_D);
		delay(20);
		line_on(EL_G);
		delay(20);
		
	}
	
	line_off(EL_D);
	line_off(EL_G);
	
	for (x = 0; x < 4; x++)
	{
		pulse(EL_A,250);
		delay(100);
	}
	
	delay(1000);
}
#endif

void setup (void)
{
	for (int i=EL_A; i<EL_MAX; i++)
		pinMode(i, OUTPUT);

	pinMode(10, OUTPUT);
	for (int x = 0; x < 2; x++)
	{
		line_on(EL_F);
		delay(20);
		line_on(EL_B);
		delay(20);
		line_on(EL_H);
		delay(20);
		line_on(EL_E);
		delay(20);
		line_on(EL_C);
		delay(20);
		line_on(EL_A);
		delay(20);
		line_on(EL_D);
		delay(20);
		line_on(EL_G);
		delay(20);
		
	}
	line_off(EL_D);
	line_off(EL_G);
}



void pulse(char line, int speed)
{
	int x;
	
	for (x = 0; x < 10000; x+=speed)
	{
		line_on(line);
		delayMicroseconds(x + 100);
		line_off(line);
		delayMicroseconds(10000 - x);
	}

	for (x = 0; x < 10000; x+=speed)
	{
		line_on(line);
		delayMicroseconds(10100 - x);
		line_off(line);
		delayMicroseconds(x + 100);
	}
	
}


void line_on(char line)//send EL_A through EL_H
{
	
	if (line_on_2 != 0)
		line_off(line_on_2);//can't have more than one line on at a time
	
	//keep track of what's on and in what sequence
	line_on_2 = line_on_1;
	line_on_1 = line;

	digitalWrite(line, LOW);
}


void line_off(char line)//send EL_A through EL_H
{
	char temp;
	
	if (line == line_on_2) line_on_2 = 0;
	else if (line == line_on_1)
	{
		line_on_1 = line_on_2;
		line_on_2 = 0;
	}
	
	digitalWrite(line, HIGH);
}

void cycle(int speed)
{
	int x;
	
	if (speed > 32000) speed = 32000;
	
	for (x=EL_A; x<EL_MAX; x++)
	{
		line_on(x);
		delayMicroseconds(32100 - speed);
	}
	
	for (x = EL_H; x >= EL_A; x--)
	{
		line_on(x);
		delayMicroseconds(32100 - speed);
	}
	
	line_off(EL_B);
	delayMicroseconds(32100 - speed);
	line_off(EL_A);
}



