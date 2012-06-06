#include "msp430g2553.h"
#include "stdlib.h"

#define BUTTON_R		BIT3
#define BUTTON_G		BIT4
#define BUTTON_B		BIT5

#define BUTTON_START	BIT5

#define LED_R			BIT0
#define LED_G			BIT1
#define LED_B			BIT2

#define LED_RED			BIT0
#define LED_GREEN		BIT6

#define LED_Y_1			BIT7
#define LED_Y_2			BIT3
#define LED_Y_3			BIT4
#define LED_Y_4			BIT1
#define LED_Y_5			BIT0

#define LED_Y_1_OUT		P1OUT
#define LED_Y_2_OUT		P2OUT
#define LED_Y_3_OUT		P2OUT
#define LED_Y_4_OUT		P2OUT
#define LED_Y_5_OUT		P2OUT

#define LED_Y_1_DIR		P1DIR
#define LED_Y_2_DIR		P2DIR
#define LED_Y_3_DIR		P2DIR
#define LED_Y_4_DIR		P2DIR
#define LED_Y_5_DIR		P2DIR


#define TIMER_SECOND	15635
#define TIMER_100MS		1564

#define TIME_TO_ENABLE_BUTTON	5000

volatile int gTicksToNextStep = 0;
volatile int gTicksToEnableButton = 0;
