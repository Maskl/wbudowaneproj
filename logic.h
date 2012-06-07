volatile int gState = STATE_NULL; // Aktualny stan w ktorym znajduje sie gra.
volatile int gIsBreak = 0; // Czy wyswietlana jest dioda czy przerwa pomiedzy nimi (w trakcie odtwarzania sekwencji).
volatile int gSequence[LEVELS_NUM]; // Sekwencja do pokazania, bedzie generowana.
volatile int gSequenceStep = 0; // Ktory etap sekwencji (wyswietlany lub wprowadza gracz).
volatile int gLevel = 1; // Jak dluga sekwencje gracz bedzie musial powtorzyc.
volatile int gSpeed = 3; // Szybkosc gry z przedzialu [1, 5].

void NextStep();
void ConfigureNextState(int state);

// Generacja tablicy z sekwencja, wykonywana przy kazdym ponownym uruchomieniu aplikacji
void GenerateSequenceTable()
{
	int i = 0;
	for (i = 0; i < LEVELS_NUM; ++i)
	{
		gSequence[i] = rand() % 3 + LED_R;
	}
}

// Ustawianie wartosci na zoltych diodach (wynik, aktualny poziom)
// Jako parametry nalezy podac LED_ON, LED_OFF lub 0 (jezeli stan diody nie ma zostac zmieniony)
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

// Procedura wyswietlajaca wybrana szybkosc gry na zoltych diodach
void ShowSpeedOnYellowLeds(int speed)
{
	SetYellowLeds(
			(speed > 0) ? LED_ON : LED_OFF,
			(speed > 1) ? LED_ON : LED_OFF,
			(speed > 2) ? LED_ON : LED_OFF,
			(speed > 3) ? LED_ON : LED_OFF,
			(speed > 4) ? LED_ON : LED_OFF);
}

// Procedura wyswietlajaca aktualny poziom na zoltych diodach (binarnie)
void ShowLevelOnYellowLeds(int level)
{
	SetYellowLeds(
			((level & 1 ) > 0) ? LED_ON : LED_OFF,
			((level & 2 ) > 0) ? LED_ON : LED_OFF,
			((level & 4 ) > 0) ? LED_ON : LED_OFF,
			((level & 8 ) > 0) ? LED_ON : LED_OFF,
			((level & 16) > 0) ? LED_ON : LED_OFF);
}

// Zapalenie jednej z kolorowych diod, wylaczenie pozostalych
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

/// Inicjalizacja kolejnyego stanu gry
void ConfigureNextState(int state)
{
	gState = state;

	switch (state)
	{
		// Stan ustawiania predkosci gry, czekamy wylacznie na przerwania od przyciskow.
		case STATE_BEFORE_START:
		{
			ShowColorLed(LED_NONE);
			ShowSpeedOnYellowLeds(gSpeed);
			gTicksToNextStep = -1;
		}
		break;

		// Poczatek pokazywania sekwencji, ktora gracz ma powtorzyc.
		// Zapalenie wszystkich zoltych diod. Pol sekundy przerwy na poczatku.
		case STATE_SHOW_SEQUENCE:
		{
			gIsBreak = 0;
			ShowLevelOnYellowLeds(31);
			gTicksToNextStep = TIMER_SECOND / 2;
		}
		break;

		// Sekwencja zostala pokazana, teraz gracz musi ja powtorzyc.
		// Na zoltych diodach pokazanie aktualnego poziomu gry, czekamy na przerwania od przyciskow.
		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			ShowLevelOnYellowLeds(gLevel);
			ShowColorLed(LED_NONE);
			gTicksToNextStep = -1;
			gSequenceStep = 0;
		}

		// Przegrana, przechodzimy od razu do mrugania diodami.
		case STATE_GAME_OVER:
		{
			NextStep();
		}

		// To samo robimy w przypadku wygranej.
		case STATE_WIN:
		{
			NextStep();
		}
	}
}

/// Kolejne zdarzenie w logice gry.
void NextStep()
{
	switch (gState)
	{
		// Stan "zerowy" - przejscie do stanu ustawiania predkosci gry.
		case STATE_NULL:
		{
			ConfigureNextState(STATE_BEFORE_START);
		}
		break;

		// Pokazywanie sekwencji do powtorzenia na kolorowych diodach LED.
		case STATE_SHOW_SEQUENCE:
		{
			// W tym stanie mamy serie zapalen kolejnych diod LED.  Jednak aby gracz mogl
			// wygodnie grac nalezy pomiedzy zapaleniami kolejnych diod zrobic chwile przerwy
			// podczas ktorej wszystkie diody beda wylaczone.
			// (Inaczej gracz moze nie wiedziec czy ma nadusic przycisk 2 czy 3 razu).
			if (!gIsBreak)
			{
				// Odczytanie ktora diode musimy zapalic
				int ledToShow = gSequence[gSequenceStep++];

				// Ewentualnie konczymy pokazywanie sekwencji jezeli pokazalismy juz caly ciag.
				if (gSequenceStep > gLevel)
				{
					ConfigureNextState(STATE_WAIT_FOR_PLAYER_REPEAT);
					break;
				}

				// Zapalenie odpowiedniej diody i poczekanie odpowiedniej ilosci czasu.
				ShowColorLed(ledToShow);
				gTicksToNextStep = TIMER_SECOND * 2 / gSpeed;
			}
			else
			{
				// Wylaczenie diod i poczekanie co najwyzej sekundy.
				ShowColorLed(LED_NONE);
				gTicksToNextStep = TIMER_SECOND / gSpeed;
			}

			gIsBreak = (gIsBreak + 1) % 2;
		}
		break;

		// W stanie powtarzania przez gracza sekwencji ponizszy blok sluzy wylacznie
		// do zgaszenia wszystkich diod (w zalozeniach po naduszeniu przycisku na
		// moment miala sie zapalic odpowiadajaca mu kolorowa dioda).
		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			ShowColorLed(LED_NONE);
			gTicksToNextStep = -1;
		}
		break;

		// Stan przegranej - losowe mruganie wszystkimi kolorowymi diodami.
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

		// Stan wygranej - mruganie zielona dioda.
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


/// Reakcja na nacisniecie przycisku.
void ButtonPressed(int buttonNum)
{
	switch (gState)
	{
		// Ustawianie szybkosci gry
		case STATE_BEFORE_START:
		{
			if (buttonNum == BUTTON_R)
			{
				if (gSpeed > 1)
					gSpeed--;

				ShowSpeedOnYellowLeds(gSpeed);
			}

			if (buttonNum == BUTTON_G)
			{
				gSpeed = 3;
				ShowSpeedOnYellowLeds(gSpeed);
			}

			if (buttonNum == BUTTON_B)
			{
				if (gSpeed < 5)
					gSpeed++;

				ShowSpeedOnYellowLeds(gSpeed);
			}

			// Rozpoczecie nowej gry.
			if (buttonNum == BUTTON_START)
			{
				gLevel = 1;
				gSequenceStep = 0;
				GenerateSequenceTable();
				ConfigureNextState(STATE_SHOW_SEQUENCE);
			}
		}
		break;

		// Stan powtarzania sekwencji przez gracza.
		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			// Restart gry.
			if (buttonNum == BUTTON_START)
			{
				ConfigureNextState(STATE_BEFORE_START);
				return;
			}

			// Odczytanie diody, ktora powinna byc wybrana.
			int ledToSelect = gSequence[gSequenceStep++];

			// Oraz tej ktora faktycznie zostala wybrana.
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
				// Zapalam na 0.2 sekundy wybrana diode.
				ShowColorLed(ledSelected);
				gTicksToNextStep = TIMER_SECOND / 5;

				// Czy dodatkowo wprowadzil juz cala sekwencje?
				if (gSequenceStep >= gLevel)
				{
					// Czyli wchodzimy na wyzszy poziom
					gLevel++;

					// Oraz ewentualnie wygrywamy
					if (gLevel > LEVELS_NUM)
					{
						ConfigureNextState(STATE_WIN);
						return;
					}

					// A jezeli nie wygrywamy to pokazujemy kolejna, dluzsza, sekwencje.
					gSequenceStep = 0;
					ConfigureNextState(STATE_SHOW_SEQUENCE);
				}
			}
			else
			{
				// Gracz wybral zly przycisk - przegrana.
				ConfigureNextState(STATE_GAME_OVER);
			}
		}
		break;

		// W pozostalych stanach naduszenie przycisku w gornym - prawym rogu restartuje gre.
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
