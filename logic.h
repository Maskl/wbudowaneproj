#define WITHOUT_CHANGES -1
#define STATE_BEFORE_START 0
#define STATE_SHOW_SEQUENCE 1
#define STATE_SHOW_SEQUENCE_BREAK 2
#define STATE_WAIT_FOR_PLAYER_REPEAT 3
#define STATE_LOOSE 4

volatile int gState = STATE_BEFORE_START; // Aktualny stan w ktorym znajduje sie gra.

volatile int gSequence[] = {0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0}; // Sekwencja do pokazania, bedzie generowana.
volatile int gSequenceStep = 0; // Ktory etap sekwencji (wyswietlany lub wprowadza gracz).
volatile int gLevel = 0; // Jak dluga sekwencje gracz bedzie musial powtorzyc.

/// Pomocnicza procedura - Ustawienie jaki ma byc kolejny krok gry.
void ConfigureNextState(int state, int ticksToNextStep)
{
	if (state >= 0)
		gState = state;

	if (ticksToNextStep >= 0)
		gTicksToNextStep = ticksToNextStep;
}

/// Kolejne zdarzenie w logice gry - gdzies wewnatrz trzeba wywolac 'ConfigureNextState' by ponownie tu trafic za odpowiednia ilosc czasu.
void NextStep()
{
	return;
	switch (gState)
	{
		// Przed rozpoczeciem gry mruganie diodami.
		case STATE_BEFORE_START:
		{
			P1OUT ^= LED_GREEN;
			P1OUT ^= LED_RED;
			ConfigureNextState(WITHOUT_CHANGES, TIMER_SECOND);
		}
		break;

		// Pokazujemy kolejna diode z sekwencji i przechodzimy to stanu pauzy.
		case STATE_SHOW_SEQUENCE:
		{
			int whichOne = gSequence[gSequenceStep++];
			if (whichOne == 0)
				P1OUT = LED_0;
			else
				P1OUT = LED_1;

			ConfigureNextState(STATE_SHOW_SEQUENCE_BREAK, TIMER_SECOND);
		}
		break;

		// Przez chwile nie pokazujemy nic - trzeba zrobic przerwe pomiedzy kolejnymi wyswietlanymi diodami.
		case STATE_SHOW_SEQUENCE_BREAK:
		{
			if (gSequenceStep <= gLevel)
			{
				// Chwila przerwy i pokazujemy kolejna diode.
				P1OUT = 0;
				ConfigureNextState(STATE_SHOW_SEQUENCE, TIMER_SECOND);
			}
			else
			{
				// Jezeli pokazalismy cala sekwencje to konczymy.
				P1OUT = 0;
				gSequenceStep = 0;
				ConfigureNextState(STATE_WAIT_FOR_PLAYER_REPEAT, TIMER_SECOND);
			}
		}
		break;

		// Czekamy na gracza - dla przykladu mrugam szybko diodami debugowymi i wylaczam duze diody (zapalane po naduszeniu przycisku).
		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			P1OUT &= ~LED_0;
			P1OUT &= ~LED_1;
			P1OUT ^= LED_GREEN;
			P1OUT ^= LED_RED;
			ConfigureNextState(WITHOUT_CHANGES, TIMER_100MS * 5);
		}
		break;

		// Stan przegranej - szybkie mruganie czerwona dioda.
		case STATE_LOOSE:
		{
			P1OUT &= ~LED_GREEN;
			P1OUT ^= LED_RED;
			ConfigureNextState(WITHOUT_CHANGES, TIMER_100MS);
		}
		break;
	}
}


/// Reakcja na klikniecie przycisku.
void ButtonPressed(int buttonNum)
{

	if (buttonNum == 0)
		P1OUT = BIT0;
	if (buttonNum == 1)
		P1OUT = BIT1;
	if (buttonNum == 2)
		P1OUT = BIT2;

	return;

	switch (gState)
	{
		// Dowolny przycisk rozpoczyna gre.
		case STATE_BEFORE_START:
		{
			// todo: losowanie tablicy
			gLevel = 0;
			gSequenceStep = 0;
			ConfigureNextState(STATE_SHOW_SEQUENCE, TIMER_SECOND);
		}
		break;

		// Etap powtarzania przez gracza - sprawdzamy czy naduszono poprawny przycisk.
		case STATE_WAIT_FOR_PLAYER_REPEAT:
		{
			// Zapalamy na moment duza diode - by widziec czy przycisk sie wcisnal poprawnie.
			if (buttonNum == 0)
				P1OUT = LED_0;
			else
				P1OUT = LED_1;

			// Sprawdzenie czy wybrano odpowiedni przycisk.
			if (buttonNum == gSequence[gSequenceStep])
			{
				// Dobrze. Czy juz powtorzylismy cala sekwencje?
				if (++gSequenceStep > gLevel)
				{
					// Tak. Wydluzamy sekwencje o jedno mrugniecie dioda.
					++gLevel;
					gSequenceStep = 0;
					ConfigureNextState(STATE_SHOW_SEQUENCE_BREAK, TIMER_100MS);
				}

				// Udalo sie dobrze nadusic przycisk, ale to jeszcze nie koniec powtarzania sekwencji.
				ConfigureNextState(WITHOUT_CHANGES, TIMER_100MS);
			}
			else
			{
				// Przegrana.
				ConfigureNextState(STATE_LOOSE, 0);
			}
		}
		break;
	}
}
