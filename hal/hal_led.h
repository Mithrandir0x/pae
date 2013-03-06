/**
 * @file hal_led.h
 */

#ifndef _HAL_LED_
#define _HAL_LED_

#include <msp430x54xa.h>

#define LED_S1 BIT0 // P1.0
#define LED_S2 BIT1 // P1.1
#define LED_SX_ALL ( BIT1 | BIT0 )

#define LED_R1 BIT0 // P4.0
#define LED_R2 BIT1 // P4.1
#define LED_R3 BIT2 // P4.2
#define LED_R4 BIT3 // P4.3
#define LED_R5 BIT4 // P4.4
#define LED_R6 BIT5 // P4.5
#define LED_R7 BIT6 // P4.6
#define LED_R8 BIT7 // P4.7
#define LED_RX_ALL 0xFF // ( LED_R8 | ... | LED_R1 )

void halLed_sx_initialize();
void halLed_sx_setLed(int leds, int flag);
void halLed_sx_toggleLed(int leds);

void halLed_rx_initialize();
void halLed_rx_setLed(int leds, int flag);
void halLed_rx_toggleLed(int leds);

#endif
