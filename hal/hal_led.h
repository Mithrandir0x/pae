/**
 * @author olopezsa13
 * @file hal_led.h
 */

#ifndef _HAL_LED_
#define _HAL_LED_

#define LED_S1 BIT0 // P1.0
#define LED_S2 BIT1 // P1.1
#define LED_SX_ALL ( BIT1 | BIT0 )

#define LED_A1 BIT0 // P4.0
#define LED_A2 BIT1 // P4.1
#define LED_A3 BIT2 // P4.2
#define LED_A4 BIT3 // P4.3
#define LED_A5 BIT4 // P4.4
#define LED_A6 BIT5 // P4.5
#define LED_A7 BIT6 // P4.6
#define LED_A8 BIT7 // P4.7
#define LED_AX_ALL 0xFF // ( LED_A8 | ... | LED_A1 )

void halLed_sx_initialize();
void halLed_sx_setLed(char leds, char flag);
void halLed_sx_toggleLed(char leds);

#endif
