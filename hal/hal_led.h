/**
 * @author olopezsa13
 * @file hal_led.h
 */

#ifndef _HAL_LED_
#define _HAL_LED_

#define LED_ON  1
#define LED_OFF 0

#define LED_S1 BIT0
#define LED_S2 BIT1
#define LED_SX_ALL ( BIT1 | BIT0 )

#define LED_A1 BIT0
#define LED_A2 BIT1
#define LED_A3 BIT2
#define LED_A4 BIT3
#define LED_A5 BIT4
#define LED_A6 BIT5
#define LED_A7 BIT6
#define LED_A8 BIT7
#define LED_AX_ALL 0xFF // ( LED_A8 | ... | LED_A1 )

extern void halLed_sx_initialize();
extern void halLed_sx_setLed(char leds, char flag);
extern void halLed_sx_toggleLed(char leds);

#endif
