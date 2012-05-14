

int LED = 13;

int xStep = 2;
int xDir = 3;
int xMS1 = 4;
int xMS2 = 5;

int yStep = 6;
int yDir = 7;
int yMS1 = 8;
int yMS2 = 9;

int steps;
int dir;

int theDelay;
void setup()
{
	pinMode(LED, OUTPUT);      // sets the digital pin as output
	digitalWrite(xStep,LOW);
	pinMode(xStep, OUTPUT);

	digitalWrite(xDir,LOW);
	pinMode(xDir, OUTPUT);

	digitalWrite(xMS1,LOW);
	pinMode(xMS1, OUTPUT);

	digitalWrite(xMS2,LOW);
	pinMode(xMS2, OUTPUT);

	digitalWrite(yStep,LOW);
	pinMode(yStep, OUTPUT);

	digitalWrite(yDir,LOW);
	pinMode(yDir, OUTPUT);

	digitalWrite(yMS1,LOW);
	pinMode(yMS1, OUTPUT);

	digitalWrite(yMS2,LOW);
	pinMode(yMS2, OUTPUT);

	theDelay = 5000;
	steps = 0;
	dir = 0;
}

void loop()
{
	
	digitalWrite(xStep, HIGH);
	digitalWrite(LED, HIGH);
	delayMicroseconds(theDelay);

	digitalWrite(xStep, LOW);
	digitalWrite(LED, LOW);
	delayMicroseconds(theDelay);

	if (++steps == 200)
	{
		steps = 0;
		dir = dir ^ 1;
		digitalWrite(xDir, dir);
		delay(1000);
	}
}
