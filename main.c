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
	P1DIR |= LED_R + LED_G + LED_B;
	P1DIR |= LED_R + LED_G + LED_B;

	LED_Y_1_DIR |= LED_Y_1;
	LED_Y_2_DIR |= LED_Y_2;
	LED_Y_3_DIR |= LED_Y_3;
	LED_Y_4_DIR |= LED_Y_4;
	LED_Y_5_DIR |= LED_Y_5;

	// Ustawienie przyciskow (wlaczenie przerwan, [?]Enable Pull up / Pull down, [?]pull up).
	P1IE  |= BUTTON_R + BUTTON_G + BUTTON_B;
	P1REN |= BUTTON_R + BUTTON_G + BUTTON_B;
	P1OUT |= BUTTON_R + BUTTON_G + BUTTON_B;
	P2IE  |= BUTTON_START;
	P2REN |= BUTTON_START;
	P2OUT |= BUTTON_START;

	// [?]Prawdopodobnie czesc z ponizszych jest zbedna - chociazby dlatego, ze wyrzucilem obsluge przerwan NMI (czymkolwiek to nie jest...).
	// Enable NMI function of the RST pin
	// WDTTMSEL = Interval timer mode
	// clock source is SMCLK (1MHz or 1000000Hz)
	// Interval is clock /64 (15625 interrupts per second)
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

void clearButtons()
{
	P1IFG = 0;
	P2IFG = 0;
	P1OUT |= BUTTON_R + BUTTON_G + BUTTON_B;
	P2OUT |= BUTTON_START;
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

	// Obsluga przycisku - wyczyszczenie flagi przerwania, ustawienie czasu do kolejnego mozliwego klikniecia i odpalenie odpowiedniej funkcji obslugi.
	if (P1IFG & BUTTON_R)
	{
		clearButtons();
		gTicksToEnableButton = TIME_TO_ENABLE_BUTTON;
		ButtonPressed(1);
	}

	// Obsluga drugiego przycisku.
	if (P1IFG & BUTTON_G)
	{
		clearButtons();
		gTicksToEnableButton = TIME_TO_ENABLE_BUTTON;
		ButtonPressed(2);
	}

	// Obsluga drugiego przycisku.
	if (P1IFG & BUTTON_B)
	{
		clearButtons();
		gTicksToEnableButton = TIME_TO_ENABLE_BUTTON;
		ButtonPressed(3);
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

	// Obsluga przycisku - wyczyszczenie flagi przerwania, ustawienie czasu do kolejnego mozliwego klikniecia i odpalenie odpowiedniej funkcji obslugi.
	if (P2IFG & BUTTON_START)
	{
		clearButtons();
		gTicksToEnableButton = TIME_TO_ENABLE_BUTTON;
		ButtonPressed(0);
	}
}

/// Przerwanie odpalane 15625 razy na sekunde.
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
	if (gTicksToNextStep > 0)
		gTicksToNextStep--;

	if (gTicksToEnableButton > 0)
		gTicksToEnableButton--;

	if (gTicksToNextStep <= 0)
		NextStep();
}
