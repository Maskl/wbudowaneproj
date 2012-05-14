#include "msp430g2553.h"

int main(void)  //Main program
{
   WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

   P1DIR |= BIT0; // Set P1.0 to output and P1.3 to input direction
   P1OUT |= BIT0; // set P1.0 to Off
   P1IE |= BIT3; // P1.3 interrupt enabled
   P1IFG &= ~BIT3; // P1.3 interrupt flag cleared
   P1REN |= BIT3 ;// P1.3 Enable Pull up / Pull down
   //P1OUT = BIT3 ;// P1.3 pull up

   __bis_SR_register(GIE); // Enable all interrupts

   while(1) //Loop forever, we'll do our job in the interrupt routine...
   {}
}
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1OUT ^= BIT0;  // Toggle P1.0
    P1IFG &= ~BIT3; // P1.3 interrupt flag cleared
}
