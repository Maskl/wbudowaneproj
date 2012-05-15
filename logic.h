#define STATE_BEFORE_START 0

int gState = STATE_BEFORE_START;

void NextStep()
{
	switch (gState)
	{
		// Przed rozpoczeciem gry mruganie diodami
		case STATE_BEFORE_START:
			P1OUT ^= LED_GREEN;
			P1OUT ^= LED_RED;
			gTicksToNextStep = TIMER_SECOND;
		break;
	}
}





void ButtonPressed(int buttonNum)
{
	if (buttonNum == 0)
		P1OUT ^= LED_0;
	else
		P1OUT ^= LED_1;
}
