#include "def.h"
#include "logic.h"

int main(void)
{
	// Zatrzymanie watchdog timera
	WDTCTL = WDTPW + WDTHOLD;

	// Ustawienie zegarow na domyslne 1MHz
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;

	// Ustawienie wszystkich wyjsc domyslnie jako wejsciowe, wyzerowanie wszystkich pinow oraz flag przerwan.
	P1DIR = P2DIR = 0;
	P1OUT = P2OUT = 0;
	P1IFG = P2IFG = 0;

	// Zapalenie diod na plytce (ustawienie pinow jako wyjsciowych + wyslanie jedynek na te piny).
	P1DIR |= LED_RED + LED_GREEN;
	P1OUT |= LED_RED;

	// Ustawienie diod LED na pinach 1.1 i 1.2 (ustawienie pinow jako wyjsciowych + wyslanie jedynek na te piny).
	P1DIR |= LED_0 + LED_1;
	P1OUT |= LED_0 + LED_1;

	// Ustawienie przyciskow na pinach 2.4 i 2.5 (wlaczenie przerwan, [?]Enable Pull up / Pull down, [?]pull up).
	P2IE  |= BUTTON_0 + BUTTON_1;
	P2REN |= BUTTON_0 + BUTTON_1;
	P2OUT |= BUTTON_0 + BUTTON_1;

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

/// Przerwanie odpalane po kliknieciu dowolnego przycisku z portu 2
#pragma vector=PORT2_VECTOR
__interrupt void port2_buttons(void)
{
	// Nie zezwalam na zbyt czeste klikanie przyciskow.
	if (gTicksToEnableButton > 0)
	{
		P2IFG = 0;
		return;
	}

	// Obsluga przycisku - wyczyszczenie flagi przerwania, ustawienie czasu do kolejnego mozliwego klikniecia i odpalenie odpowiedniej funkcji obslugi.
	if (P2IFG & BUTTON_0)
	{
		P2IFG = 0;
		gTicksToEnableButton = TIME_TO_ENABLE_BUTTON;
		ButtonPressed(0);
	}

	// Obsluga drugiego przycisku.
	if (P2IFG & BUTTON_1)
	{
		P2IFG = 0;
		gTicksToEnableButton = TIME_TO_ENABLE_BUTTON;
		ButtonPressed(1);
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

	//if (gTicksForSeed > 100)
	//	gTicksForSeed = 0;

	if (gTicksToNextStep <= 0)
		NextStep();
}
