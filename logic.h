#define STATE_BEFORE_START 0
#define STATE_SHOW_SEQUENCE 1
#define STATE_SHOW_SEQUENCE_BREAK 2
#define STATE_WAIT_FOR_PLAYER_REPEAT 3
#define STATE_LOOSE 4

volatile int gState = STATE_BEFORE_START;

volatile int gSequence[] = {0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0};
volatile int gLevel = 0; // Jak dluga sekwencje gracz bedzie musial powtorzyc
volatile int gSeed = 0; // Ziarno losowania w tej grze.
volatile int gShowingSequenceStep = 0; // Etap pokazywania sekwencji.
volatile int gRepeatingSequenceStep = 0; // Etap powtatarzania przez gracza sekwencji.

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
				gRepeatingSequenceStep = 0;
				gTicksToNextStep = TIMER_SECOND;
				gState = STATE_WAIT_FOR_PLAYER_REPEAT;
			}
		}
		break;

		// Przed rozpoczeciem gry mruganie diodami
		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			P1OUT &= ~LED_0;
			P1OUT &= ~LED_1;
			P1OUT ^= LED_GREEN;
			P1OUT ^= LED_RED;
			gTicksToNextStep = TIMER_100MS * 5;
		}
		break;

		case STATE_LOOSE:
		{
			P1OUT &= ~LED_GREEN;
			P1OUT ^= LED_RED;
			gTicksToNextStep = TIMER_100MS;
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
			gLevel = 0;
			gShowingSequenceStep = 0;

			gState = STATE_SHOW_SEQUENCE;
			gTicksToNextStep = TIMER_SECOND;
		break;

		// Przed rozpoczeciem gry mruganie diodami
		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			if (buttonNum == 0)
				P1OUT = LED_0;
			else
				P1OUT = LED_1;

			if (buttonNum == gSequence[gRepeatingSequenceStep])
			{
				// Dobrze.
				gTicksToNextStep = TIMER_100MS;
				gRepeatingSequenceStep++;

				if (gRepeatingSequenceStep > gLevel)
				{
					++gLevel;
					gShowingSequenceStep = 0;
					gState = STATE_SHOW_SEQUENCE_BREAK;
				}
			}
			else
			{
				gState = STATE_LOOSE;
				gTicksToNextStep = TIMER_SECOND * 5;
			}
		}
		break;
	}
}
