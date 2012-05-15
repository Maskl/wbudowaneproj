#include "msp430g2553.h"
volatile int ticks = 0;

int main(void) //Main program
{
	WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

	// set up the clocks for the factory preset 1MHz
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;

	/* P1DIR |= BIT0; // Set P1.0 to output and P1.3 to input direction
	 // P1DIR |= BIT1; // Set P1.0 to output and P1.3 to input direction
	 //P1OUT |= BIT0; // set P1.0 to Off
	 // P1OUT |= BIT1; // set P1.0 to Off
	 P1IE |= BIT1; // P1.3 interrupt enabled
	 P1IFG &= ~BIT1; // P1.3 interrupt flag cleared
	 P1REN |= BIT1 ;// P1.3 Enable Pull up / Pull down
	 P1OUT = BIT1 ;// P1.3 pull up
	 // P1OUT |= BIT1; // set P1.0 to Off
	 */

	P1DIR = BIT1 + BIT2;
	P1OUT = BIT1 + BIT2;

	P2IE |= BIT4; // P1.3 interrupt enabled
	P2IFG &= ~BIT4; // P1.3 interrupt flag cleared
	P2REN |= BIT4; // P1.3 Enable Pull up / Pull down
	P2OUT |= BIT4; // P1.3 pull up

	P2IE |= BIT5; // P1.3 interrupt enabled
	P2IFG &= ~BIT5; // P1.3 interrupt flag cleared
	P2REN |= BIT5; // P1.3 Enable Pull up / Pull down
	P2OUT |= BIT5; // P1.3 pull up

	// Enable NMI function of the RST pin
	// WDTTMSEL = Interval timer mode
	// clock source is SMCLK (1MHz or 1000000Hz)
	// Interval is clock /64 (15625 interrupts per second)
	WDTCTL = WDTPW + WDTNMI + WDTTMSEL + WDTCNTCL + WDTIS1 + WDTIS0;
	IE1 |= WDTIE;
	__bis_SR_register(LPM1_bits + GIE);
	// Enable all interrupts

	while (1) //Loop forever, we'll do our job in the interrupt routine...
	{
	}
}

#pragma vector=WDT_VECTOR
__interrupt void wdt_trigger(void)
{
// WDT is triggered 15625 times per second
	ticks++;

		if (ticks >= 15635) {
			P1OUT ^= BIT1;
			ticks = 0;
		}
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_1(void) {
	unsigned char flags = P2IFG & P2IE;

	if (flags & BIT4) {
		P1OUT ^= BIT1; // Toggle P1.0
		P2IFG &= ~BIT4; // P1.3 interrupt flag cleared
		P2IFG &= ~BIT5; // P1.3 interrupt flag cleared
		return;
	}

	if (flags & BIT5) {
		P1OUT ^= BIT2; // Toggle P1.0
		P2IFG &= ~BIT5; // P1.3 interrupt flag cleared
	}

}
