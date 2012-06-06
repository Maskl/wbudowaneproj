#include "msp430g2553.h"
#include "stdlib.h"

#define BUTTON_R_BIT	BIT5
#define BUTTON_G_BIT	BIT4
#define BUTTON_B_BIT	BIT3

#define BUTTON_START_BIT	BIT5

#define LED_R_BIT		BIT2
#define LED_G_BIT		BIT1
#define LED_B_BIT		BIT0

#define LED_Y_1_BIT		BIT7
#define LED_Y_2_BIT		BIT3
#define LED_Y_3_BIT		BIT4
#define LED_Y_4_BIT		BIT1
#define LED_Y_5_BIT		BIT0

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






#define BUTTON_R		1
#define BUTTON_G		2
#define BUTTON_B		3
#define BUTTON_START	9








volatile int gTicksToNextStep = -1;
volatile int gTicksToEnableButton = 0;
