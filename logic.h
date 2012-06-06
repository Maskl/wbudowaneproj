volatile int gState = STATE_NULL; // Aktualny stan w ktorym znajduje sie gra.
volatile int gIsBreak = 0; // Czy wyswietlana jest dioda czy przerwa pomiedzy nimi (w trakcie odtwarzania sekwencji).
volatile int gSequence[LEVELS_NUM]; // Sekwencja do pokazania, bedzie generowana.
volatile int gSequenceStep = 0; // Ktory etap sekwencji (wyswietlany lub wprowadza gracz).
volatile int gLevel = 1; // Jak dluga sekwencje gracz bedzie musial powtorzyc.
volatile int gSpeed = 3; // Szybkosc gry z przedzialu [1, 5].

void NextStep();
void ConfigureNextState(int state);

void GenerateSequenceTable()
{
	int i = 0;
	for (i = 0; i < LEVELS_NUM; ++i)
	{
		gSequence[i] = rand() % 3 + LED_R;
	}
}

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

void ShowLevelOnYellowLeds(int level)
{
	SetYellowLeds(
			((level & 1 ) > 0) ? LED_ON : LED_OFF,
			((level & 2 ) > 0) ? LED_ON : LED_OFF,
			((level & 4 ) > 0) ? LED_ON : LED_OFF,
			((level & 8 ) > 0) ? LED_ON : LED_OFF,
			((level & 16) > 0) ? LED_ON : LED_OFF);
}

void ShowColorLed(int led)
{
	if (led == LED_R)
	{
		P1OUT |= LED_R_BIT;
		P1OUT &= ~LED_G_BIT;
		P1OUT &= ~LED_B_BIT;
	}

	if (led == LED_G)
	{
		P1OUT &= ~LED_R_BIT;
		P1OUT |= LED_G_BIT;
		P1OUT &= ~LED_B_BIT;
	}

	if (led == LED_B)
	{
		P1OUT &= ~LED_R_BIT;
		P1OUT &= ~LED_G_BIT;
		P1OUT |= LED_B_BIT;
	}

	if (led == LED_NONE)
	{
		P1OUT &= ~LED_R_BIT;
		P1OUT &= ~LED_G_BIT;
		P1OUT &= ~LED_B_BIT;
	}
}

/// Pomocnicza procedura - Ustawienie jaki ma byc kolejny krok gry.
void ConfigureNextState(int state)
{
	gState = state;

	switch (state)
	{
		case STATE_BEFORE_START:
		{
			ShowColorLed(LED_NONE);
			ShowSpeedOnYellowLeds();
			gTicksToNextStep = -1;
		}
		break;

		case STATE_SHOW_SEQUENCE:
		{
			gIsBreak = 0;
			ShowLevelOnYellowLeds(31);
			gTicksToNextStep = TIMER_SECOND / 2;
		}
		break;

		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			ShowLevelOnYellowLeds(gLevel);
			ShowColorLed(LED_NONE);
			gTicksToNextStep = -1;
			gSequenceStep = 0;
		}

		case STATE_GAME_OVER:
		{
			NextStep();
		}

		case STATE_WIN:
		{
			NextStep();
		}
	}
}

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
			if (!gIsBreak)
			{
				int ledToShow = gSequence[gSequenceStep++];

				if (gSequenceStep > gLevel)
				{
					ConfigureNextState(STATE_WAIT_FOR_PLAYER_REPEAT);
					break;
				}

				ShowColorLed(ledToShow);
				gTicksToNextStep = TIMER_SECOND * 2 / gSpeed;
			}
			else
			{
				ShowColorLed(LED_NONE);
				gTicksToNextStep = TIMER_SECOND / gSpeed;
			}

			gIsBreak = (gIsBreak + 1) % 2;
		}
		break;

		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			ShowColorLed(LED_NONE);
			gTicksToNextStep = -1;
		}
		break;

		case STATE_GAME_OVER:
		{
			int r = rand() % 3;
			if (r == 0)
				ShowColorLed(LED_R);
			if (r == 1)
				ShowColorLed(LED_G);
			if (r == 2)
				ShowColorLed(LED_B);

			gTicksToNextStep = TIMER_SECOND / 30;
		}
		break;

		case STATE_WIN:
		{
			if ((rand() % 2) == 1)
				ShowColorLed(LED_G);
			else
				ShowColorLed(LED_NONE);

			gTicksToNextStep = TIMER_SECOND / 10;
		}
		break;
	}

	return;
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

			if (buttonNum == BUTTON_G)
			{
				gSpeed = 3;
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
				gLevel = 1;
				gSequenceStep = 0;
				GenerateSequenceTable();
				ConfigureNextState(STATE_SHOW_SEQUENCE);
			}
		}
		break;

		// Przed rozpoczeciem gry mruganie diodami.
		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			if (buttonNum == BUTTON_START)
			{
				ConfigureNextState(STATE_BEFORE_START);
				return;
			}

			int ledToSelect = gSequence[gSequenceStep++];
			int ledSelected;

			if (buttonNum == BUTTON_R)
				ledSelected = LED_R;

			if (buttonNum == BUTTON_G)
				ledSelected = LED_G;

			if (buttonNum == BUTTON_B)
				ledSelected = LED_B;

			// Czy gracz nadusil odpowiedni przycisk?
			if (ledToSelect == ledSelected)
			{
				ShowColorLed(ledSelected);
				gTicksToNextStep = TIMER_SECOND / 5;

				// Dodatkowo prowadzil juz cala sekwencje.
				if (gSequenceStep >= gLevel)
				{
					gLevel++;
					if (gLevel > LEVELS_NUM)
					{
						ConfigureNextState(STATE_WIN);
						return;
					}

					gSequenceStep = 0;
					ConfigureNextState(STATE_SHOW_SEQUENCE);
				}
			}
			else
			{
				ConfigureNextState(STATE_GAME_OVER);
			}
		}
		break;

		default:
		{
			if (buttonNum == BUTTON_START)
			{
				ConfigureNextState(STATE_BEFORE_START);
				return;
			}
		}
		break;
	}
}
