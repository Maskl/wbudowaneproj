#define STATE_NULL -1
#define STATE_BEFORE_START 0
#define STATE_SHOW_SEQUENCE 1
#define STATE_WAIT_FOR_PLAYER_REPEAT 3
#define STATE_LOOSE 4

volatile int gState = STATE_NULL; // Aktualny stan w ktorym znajduje sie gra.

volatile int gSequence[] = {0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0}; // Sekwencja do pokazania, bedzie generowana.
volatile int gSequenceStep = 0; // Ktory etap sekwencji (wyswietlany lub wprowadza gracz).
volatile int gLevel = 0; // Jak dluga sekwencje gracz bedzie musial powtorzyc.

volatile int gSpeed = 3;


void SetYellowLeds(int led1, int led2, int led3, int led4, int led5)
{
	if (led1 == LED_ON)
		LED_Y_1_OUT |= LED_Y_1_BIT;
	else if (led1 == LED_OFF)
		LED_Y_1_OUT &= ~LED_Y_1_BIT;

	if (led2 == LED_ON)
		LED_Y_2_OUT |= LED_Y_2_BIT;
	else if (led2 == LED_OFF)
		LED_Y_2_OUT &= ~LED_Y_2_BIT;

	if (led3 == LED_ON)
		LED_Y_3_OUT |= LED_Y_3_BIT;
	else if (led3 == LED_OFF)
		LED_Y_3_OUT &= ~LED_Y_3_BIT;

	if (led4 == LED_ON)
		LED_Y_4_OUT |= LED_Y_4_BIT;
	else if (led4 == LED_OFF)
		LED_Y_4_OUT &= ~LED_Y_4_BIT;

	if (led5 == LED_ON)
		LED_Y_5_OUT |= LED_Y_5_BIT;
	else if (led5 == LED_OFF)
		LED_Y_5_OUT &= ~LED_Y_5_BIT;
}


void ShowSpeedOnYellowLeds()
{
	SetYellowLeds(
			(gSpeed > 0) ? LED_ON : LED_OFF,
			(gSpeed > 1) ? LED_ON : LED_OFF,
			(gSpeed > 2) ? LED_ON : LED_OFF,
			(gSpeed > 3) ? LED_ON : LED_OFF,
			(gSpeed > 4) ? LED_ON : LED_OFF);
}

void ShowLevelOnYellowLeds()
{
	SetYellowLeds(
			(gSpeed == 1) ? LED_ON : LED_OFF,
			(gSpeed == 2) ? LED_ON : LED_OFF,
			(gSpeed == 3) ? LED_ON : LED_OFF,
			(gSpeed == 4) ? LED_ON : LED_OFF,
			(gSpeed == 5) ? LED_ON : LED_OFF);
}

void NextStep();
void ConfigureNextState(int state);


/// Kolejne zdarzenie w logice gry - gdzies wewnatrz trzeba wywolac 'ConfigureNextState' by ponownie tu trafic za odpowiednia ilosc czasu.
void NextStep()
{
	switch (gState)
	{
		// Przed rozpoczeciem gry mruganie diodami.
		case STATE_NULL:
		{
			ConfigureNextState(STATE_BEFORE_START);
		}
		break;

		// Przed rozpoczeciem gry mruganie diodami.
		case STATE_SHOW_SEQUENCE:
		{
			ShowLevelOnYellowLeds();
			gSequenceStep++;
			if (gSequenceStep % 3 == 0)
				P1OUT |= LED_R_BIT;
			if (gSequenceStep % 3 == 1)
				P1OUT |= LED_G_BIT;
			if (gSequenceStep % 3 == 2)
				P1OUT |= LED_B_BIT;
			gTicksToNextStep = TIMER_SECOND;
		}
		break;
	}

	return;
}


/// Pomocnicza procedura - Ustawienie jaki ma byc kolejny krok gry.
void ConfigureNextState(int state)
{
	gState = state;

	switch (state)
	{
		case STATE_BEFORE_START:
		{
			ShowSpeedOnYellowLeds();
			gTicksToNextStep = -1;
		}
		break;

		case STATE_SHOW_SEQUENCE:
		{
			NextStep();
		}
		break;
	}
}


/// Reakcja na klikniecie przycisku.
void ButtonPressed(int buttonNum)
{
	switch (gState)
	{
		case STATE_BEFORE_START:
		{
			if (buttonNum == BUTTON_R)
			{
				if (gSpeed > 1)
					gSpeed--;

				ShowSpeedOnYellowLeds();
			}

			if (buttonNum == BUTTON_B)
			{
				if (gSpeed < 5)
					gSpeed++;

				ShowSpeedOnYellowLeds();
			}

			if (buttonNum == BUTTON_START)
			{
				gLevel = 0;
				gSequenceStep = 0;
				ConfigureNextState(STATE_SHOW_SEQUENCE);
			}
		}
		break;
	}
}
