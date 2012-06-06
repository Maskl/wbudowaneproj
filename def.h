#include "msp430g2553.h"
#include "stdlib.h"

// Bity przyciskow
#define BUTTON_R_BIT		BIT5
#define BUTTON_G_BIT		BIT4
#define BUTTON_B_BIT		BIT3
#define BUTTON_START_BIT	BIT5

// Bity diod LED
#define LED_R_BIT		BIT2
#define LED_G_BIT		BIT1
#define LED_B_BIT		BIT0
#define LED_Y_1_BIT		BIT7
#define LED_Y_2_BIT		BIT3
#define LED_Y_3_BIT		BIT4
#define LED_Y_4_BIT		BIT1
#define LED_Y_5_BIT		BIT0

// Pomocnicze makra dla obslugi zoltych diod
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

// Liczba taktow na sekunde
#define TIMER_SECOND	15635

// Liczba taktow minimalnej przerwy pomiedzy kolejnymi nacisnieciami przyciskow
#define TIME_TO_ENABLE_BUTTON	5000

// Numery przyciskow
#define BUTTON_R		1
#define BUTTON_G		2
#define BUTTON_B		3
#define BUTTON_START	9

// Numery diod
#define LED_R			1
#define LED_G			2
#define LED_B			3
#define LED_NONE		0

// Pomocnicze makra dla funkcji zapalajacej zolte diody
#define LED_ON			1
#define LED_OFF			2

// Maksymalna liczba poziomow (po ktorej gracz wygrywa)
#define LEVELS_NUM 		25

// Stany gry
#define STATE_NULL 						-1
#define STATE_BEFORE_START 				0
#define STATE_SHOW_SEQUENCE 			1
#define STATE_WAIT_FOR_PLAYER_REPEAT 	2
#define STATE_GAME_OVER 				3
#define STATE_WIN 						4

// Zmienne dla watchdoga
volatile int gTicksToNextStep = 0;
volatile int gTicksToEnableButton = 0;
