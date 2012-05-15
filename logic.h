#define STATE_BEFORE_START 0
#define STATE_SHOW_SEQUENCE 1
#define STATE_SHOW_SEQUENCE_BREAK 2
#define STATE_WAIT_FOR_PLAYER_REPEAT 3

volatile int gState = STATE_BEFORE_START;

volatile int gSequence[] = {0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0};
volatile int gLevel = 0; // Jak dluga sekwencje gracz bedzie musial powtorzyc
volatile int gSeed = 0; // Ziarno losowania w tej grze.
volatile int gShowingSequenceStep = 0; // Etap pokazywania sekwencji.

void NextStep()
{
	switch (gState)
	{
		// Przed rozpoczeciem gry mruganie diodami
		case STATE_BEFORE_START:
		{
			P1OUT ^= LED_GREEN;
			P1OUT ^= LED_RED;
			gTicksToNextStep = TIMER_SECOND;
		}
		break;

		// Pokazujemy sekwencje graczowi
		case STATE_SHOW_SEQUENCE:
		{
			// Pokazanie kolejnej diody.
			int whichOne = gSequence[gShowingSequenceStep];
			if (whichOne == 0)
				P1OUT = LED_0;
			else
				P1OUT = LED_1;

			gShowingSequenceStep++;

			gTicksToNextStep = TIMER_SECOND;
			gState = STATE_SHOW_SEQUENCE_BREAK;
		}
		break;


		// Pokazujemy sekwencje graczowi
		case STATE_SHOW_SEQUENCE_BREAK:
		{
			if (gShowingSequenceStep <= gLevel)
			{
				// Chwila przerwy i pokazujemy kolejna diode.
				P1OUT = 0;
				gTicksToNextStep = TIMER_SECOND;
				gState = STATE_SHOW_SEQUENCE;
			}
			else
			{
				// Jezeli pokazalismy cala sekwencje to konczymy.
				P1OUT = 0;
				gState = STATE_WAIT_FOR_PLAYER_REPEAT;
				gTicksToNextStep = 0;
			}
		}
		break;

		// Przed rozpoczeciem gry mruganie diodami
		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			P1OUT ^= LED_GREEN;
			P1OUT ^= LED_RED;
			gTicksToNextStep = TIMER_100MS * 3;
		}
		break;
	}
}





void ButtonPressed(int buttonNum)
{
	switch (gState)
	{
		// Dowolny przycisk rozpoczyna gre
		case STATE_BEFORE_START:
			// todo: losowanie tablicy
			gLevel = 5;
			gShowingSequenceStep = 0;

			gState = STATE_SHOW_SEQUENCE;
			gTicksToNextStep = TIMER_SECOND;
		break;


		case STATE_SHOW_SEQUENCE:
			gShowingSequenceStep = 0;
		break;
	}


	//if (buttonNum == 0)
	//	P1OUT ^= LED_0;
	//else
	//	P1OUT ^= LED_1;
}
