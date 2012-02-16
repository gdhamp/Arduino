

//Define functions
//======================

boolean state=true;
int del = 10;

void loop (void)
{
	char choice;

//	del = random(300,800);
	
	digitalWrite(2, HIGH);
	delayMicroseconds(300);
	digitalWrite(2, LOW);

	delay(random(50,120));

	digitalWrite(2, HIGH);
	delayMicroseconds(300);
	digitalWrite(2, LOW);

	delay(random(50,120));

	digitalWrite(2, HIGH);
	delayMicroseconds(300);
	digitalWrite(2, LOW);

	if (random(2))
	{
		delay(random(50,120));

		digitalWrite(2, HIGH);
		delayMicroseconds(300);
		digitalWrite(2, LOW);
	}

	delay(random(50,2300));

	if(state)
	{
		state = false;
		digitalWrite(13, LOW);
	}
	else
	{
		state = true;
		digitalWrite(13, HIGH);
	}
}

void setup (void)
{
	pinMode(2, OUTPUT);
	pinMode(13, OUTPUT);
}


