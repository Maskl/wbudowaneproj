#include "def.h"
#include "logic.h"

int main(void)
{
	// Zatrzymanie watchdog timera
	WDTCTL = WDTPW + WDTHOLD;

	// Ustawienie zegarow na domyslne 1MHz
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL  = CALDCO_1MHZ;

	// Ustawienie wszystkich wyjsc domyslnie jako wejsciowe, wyzerowanie wszystkich pinow oraz flag przerwan.
	P1DIR = P2DIR = 0;
	P1OUT = P2OUT = 0;
	P1IFG = P2IFG = 0;
	P1IE  = P2IE  = 0;
	P1REN = P2REN = 0;

	// Ustawienie diod LED (ustawienie pinow jako wyjsciowych + wyslanie jedynek na te piny).
	P1DIR |= LED_R_BIT + LED_G_BIT + LED_B_BIT;
	P1DIR |= LED_R_BIT + LED_G_BIT + LED_B_BIT;

	LED_Y_1_DIR |= LED_Y_1_BIT;
	LED_Y_2_DIR |= LED_Y_2_BIT;
	LED_Y_3_DIR |= LED_Y_3_BIT;
	LED_Y_4_DIR |= LED_Y_4_BIT;
	LED_Y_5_DIR |= LED_Y_5_BIT;

	// Ustawienie przyciskow (wlaczenie przerwan, [?]Enable Pull up / Pull down, [?]pull up).
	P1IE  |= BUTTON_R_BIT + BUTTON_G_BIT + BUTTON_B_BIT;
	P1REN |= BUTTON_R_BIT + BUTTON_G_BIT + BUTTON_B_BIT;
	P1OUT |= BUTTON_R_BIT + BUTTON_G_BIT + BUTTON_B_BIT;
	P2IE  |= BUTTON_START_BIT;
	P2REN |= BUTTON_START_BIT;
	P2OUT |= BUTTON_START_BIT;

	// Ustawienie watchdoga
	WDTCTL = WDTPW + WDTNMI + WDTTMSEL + WDTCNTCL + WDTIS1 + WDTIS0;
	IE1 |= WDTIE;

	// Wlaczenie wszystkich przerwan
	__bis_SR_register(GIE);

	// Petla nieskonczona, wszystko bedzie sie dzialo wewnatrz procedur odpalanych przez przerwania
	while (1)
	{
		;
	}
}

// Procedura czyszczaca flagi przerwan na wszystkich przyciskach
void clearButtons()
{
	P1IFG = 0;
	P2IFG = 0;
	P1OUT |= BUTTON_R_BIT + BUTTON_G_BIT + BUTTON_B_BIT;
	P2OUT |= BUTTON_START_BIT;
}

/// Przerwanie odpalane po kliknieciu dowolnego przycisku z portu 1
#pragma vector=PORT1_VECTOR
__interrupt void port1_buttons(void)
{
	// Nie zezwalam na zbyt czeste klikanie przyciskow.
	if (gTicksToEnableButton > 0)
	{
		clearButtons();
		return;
	}

	// Obsluga przycisku R - wyczyszczenie flagi przerwania, ustawienie czasu do
	// kolejnego mozliwego klikniecia i odpalenie odpowiedniej funkcji obslugi.
	if (P1IFG & BUTTON_R_BIT)
	{
		clearButtons();
		gTicksToEnableButton = TIME_TO_ENABLE_BUTTON;
		ButtonPressed(BUTTON_R);
	}

	// Obsluga drugiego przycisku (G)
	if (P1IFG & BUTTON_G_BIT)
	{
		clearButtons();
		gTicksToEnableButton = TIME_TO_ENABLE_BUTTON;
		ButtonPressed(BUTTON_G);
	}

	// Obsluga trzeciego przycisku (B)
	if (P1IFG & BUTTON_B_BIT)
	{
		clearButtons();
		gTicksToEnableButton = TIME_TO_ENABLE_BUTTON;
		ButtonPressed(BUTTON_B);
	}
}

/// Przerwanie odpalane po kliknieciu dowolnego przycisku z portu 2
#pragma vector=PORT2_VECTOR
__interrupt void port2_buttons(void)
{
	// Nie zezwalam na zbyt czeste klikanie przyciskow.
	if (gTicksToEnableButton > 0)
	{
		clearButtons();
		return;
	}

	// Obsluga przycisku START/RESTART
	if (P2IFG & BUTTON_START_BIT)
	{
		clearButtons();
		gTicksToEnableButton = TIME_TO_ENABLE_BUTTON;
		ButtonPressed(BUTTON_START);
	}
}

/// Przerwanie odpalane 15625 razy na sekunde.
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
	// Dekrementuje zmienna uniemozliwiajaca zbyt czeste naciskanie przyciskow (eliminacja drgan).
	if (gTicksToEnableButton > 0)
		gTicksToEnableButton--;

	// Wywolywanie obslugi kolejnego zdarzenia po wyznaczonej liczbie taktow zegara.
	if (gTicksToNextStep > 0)
		gTicksToNextStep--;

	if (gTicksToNextStep == 0)
		NextStep();
}
