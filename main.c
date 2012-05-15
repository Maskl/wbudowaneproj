#include "msp430g2553.h"

#define BUTTON_0	BIT4
#define BUTTON_1	BIT5
#define LED_0		BIT1
#define LED_1		BIT2
#define LED_RED		BIT0
#define LED_GREEN	BIT6

volatile int gTicks = 0;

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

// Przerwanie odpalane 15625 razy na sekunde.
#pragma vector=WDT_VECTOR
__interrupt void wdt_trigger(void)
{
	gTicks++;

	if (gTicks >= 15635)
	{
		P1OUT ^= LED_GREEN;
		P1OUT ^= LED_RED;
		gTicks = 0;
	}
}

// Przerwanie odpalane po kliknieciu dowolnego przycisku z portu 2
#pragma vector=PORT2_VECTOR
__interrupt void Port_1(void)
{

	if (P2IFG & BUTTON_0)
	{
		// Wyczyszczenie flagi przerwania [?]niekoniecznie dobre, pewnie powinienem czysci tylko odpowiedni bit
		P2IFG = 0;
		P1OUT ^= LED_0;
	}

	if (P2IFG & BUTTON_1)
	{
		P2IFG = 0;
		P1OUT ^= LED_1;
	}

}
