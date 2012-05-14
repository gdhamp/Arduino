

unsigned short isrPulseCount = 0;

void add_pulse()
{
	isrPulseCount++;
	return;
}

ISR(INT0_vect)
{
	isrPulseCount++;
}


ISR(INT1_vect, ISR_NAKED)
{
	uint8_t s;
	asm (
			"push __tmp_reg__" "\n\t"
			"in __tmp_reg__,__SREG__" "\n\t"
			"push r26" "\n\t"
			"push r27" "\n\t"
			::);

	asm (
			"adiw %0,1" "\n\t"
			: "=x" (isrPulseCount)
			: "0" (isrPulseCount));
	asm (
			"pop r27" "\n\t"
			"pop r26" "\n\t"
			"out __SREG__,__tmp_reg__" "\n\t"
			"pop __tmp_reg__" "\n\t"
			"reti" "\n\t"
			::);
}

void setup()
{
	// attach interrupt to pin2, send output pin of TSL230R to arduino 2
	// call handler on each rising pulse

//	attachInterrupt(0, add_pulse, RISING);
}


void loop()
{
}




