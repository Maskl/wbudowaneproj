#include "msp430g2553.h"

volatile int gTicks = 0; // counts the WDT interrupts
volatile char mode = 0; // 0 = slow, 1 = fast, 2 = manual
volatile int debounce = 2000; // disables the buttons until WDT decreases this to 0

int gTime = 0;


void SetDebugLeds(int red, int green)
{
	//int redBit = (red != 0) ? BIT0 : 0;		// Numer diody do zapalenia lub zero.
	//if (P1OUT & BIT0 != redBit)				// Sprawdzam czy trzeba zmienic diode.
	//	P1OUT ^= BIT0;						// Zmiana wartosci diody.

	//int greenBit = (green != 0) ? BIT6 : 0;	// Numer diody do zapalenia lub zero.
	//if (P1OUT & BIT6 != greenBit)			// Sprawdzam czy trzeba zmienic diode.
	//	P1OUT ^= BIT6;						// Zmiana wartosci diody.
}

int gCurrentLed = BIT1;
void EnableNextLED(void)
{
	gCurrentLed <<= 1;						// Zmieniam numer diody do zapalenia.
	if (gCurrentLed == BIT4)				// Jak doszedlem za ostatnia.
		gCurrentLed = BIT1;					// To zmieniam na pierwsza.

	//gCurrentLed |= P1OUT & BIT0;			// Nie chce zmieniac wartosci malej czerwonej diody
	//gCurrentLed |= P1OUT & BIT6;			// i zielonej.

	P1OUT = gCurrentLed | BIT6;					// Faktyczne przypisanie wartosci diody.
}

int gStates[] =
            {
            BIT0, BIT6, BIT0, BIT6, BIT0, BIT0, BIT6, BIT0, BIT6, BIT0, BIT0, BIT6, BIT0, BIT6, BIT0, BIT0, BIT6, BIT0, BIT6, BIT0,
            BIT2, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            BIT6, 0, 0, 0, 0,
            BIT2, 0, 0, 0, 0,
            BIT4, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            BIT6, 0, 0, 0, 0,
            BIT2, 0, 0, 0, 0,
            BIT4, 0, 0, 0, 0,
            BIT3, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            BIT6, 0, 0, 0, 0,
            BIT2, 0, 0, 0, 0,
            BIT4, 0, 0, 0, 0,
            BIT3, 0, 0, 0, 0,
            BIT3, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            BIT6, 0, 0, 0, 0,
            BIT2, 0, 0, 0, 0,
            BIT4, 0, 0, 0, 0,
            BIT3, 0, 0, 0, 0,
            BIT3, 0, 0, 0, 0,
            BIT4, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            BIT6, 0, 0, 0, 0,
            BIT2, 0, 0, 0, 0,
            BIT4, 0, 0, 0, 0,
            BIT3, 0, 0, 0, 0,
            BIT3, 0, 0, 0, 0,
            BIT4, 0, 0, 0, 0,
            BIT2, 0
            -1
            };

int gStateIndex = 0, gRed = 0;
void CreateNextLEDState()
{
	if (gStates[gStateIndex] < 0)
	{
		gRed++;
		if (gRed % 3 == 0 || gRed < 13)
			P1OUT = 0;
		else
			P1OUT = BIT0 | BIT4;
		return;
	}

	P1OUT = gStates[gStateIndex];

	gStateIndex++;
}


int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;   			// Wylaczenie "watchdog timer".
	BCSCTL1 = CALBC1_1MHZ;					// Ustawienie zegarow na domyslne 1MHz.
	DCOCTL = CALDCO_1MHZ;					// Ustawienie zegarow na domyslne 1MHz.

	P1DIR = BIT0 + BIT1 + BIT2 + BIT4 + BIT5 + BIT6;	// Ustawienie pinow jako wyjsciowe (domyslnie wejsciowe).
	//P2DIR = BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;	// Ustawienie pinow jako wyjsciowe (domyslnie wejsciowe).
	P1OUT = BIT0;							// Wlaczenie malej czerwonej diody.
	//P1OUT = BIT4;
	P1SEL = 0;								// (?) Wylaczenie dodatkowych funkcji pinow.

	// set up the clocks for the LED blink

	// WDTTMSEL = Interval timer mode
	// clock source is SMCLK (1MHz or 1000000Hz)
	// Interval is clock /64 (15625 interrupts per second)
		WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTIS1 + WDTIS0;

		IE1 |= WDTIE; // enable WDT interrupt

	// set up the interrupt for the button
		P1IE |= BIT3;
		P1IES |= BIT3;
		P1IFG &= ~BIT3; // clear the interrupt flag

		_bis_SR_register(LPM1_bits + GIE); // enter low power mode w/ interrupts
}




int gAAA = 0;
#pragma vector=WDT_VECTOR
__interrupt void wdt_trigger(void)
{
	// WDT is triggered 15625 times per second
	gTicks++;
	if (debounce > 0)
	{
		debounce--;
	}


	if (gTicks >= 15635 / 4)
	{
		//CreateNextLEDState();

		//if (gAAA != P1IN)
		//	EnableNextLED();

		//gAAA = P1IN;






		gTicks = 0;

		//gTime++;
		//SetDebugLeds(gTime % 2, (gTime + 1) % 2);
	}

	if (gTicks % (15635) == 0)
	{

	}
}

#pragma vector=PORT1_VECTOR
__interrupt void button_trigger(void) {
	P1IFG &= ~BIT3; // clear the interrupt flag

	//P1OUT = BIT6;
	EnableNextLED();
}
