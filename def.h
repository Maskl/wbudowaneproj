#include "msp430g2553.h"
#include "stdlib.h"

#define BUTTON_0		BIT4
#define BUTTON_1		BIT5
#define LED_0			BIT1
#define LED_1			BIT2
#define LED_RED			BIT0
#define LED_GREEN		BIT6

#define TIMER_SECOND	15635
#define TIMER_100MS		1564

#define TIME_TO_ENABLE_BUTTON	5000

volatile int gTicksToNextStep = 0;
volatile int gTicksToEnableButton = 0;
volatile int gTicksForSeed = 0;
