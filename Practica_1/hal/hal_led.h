/**
 * @author olopezsa13
 * @file hal_led.h
 */

#ifndef _HAL_LED_
#define _HAL_LED_

#define LED_ON  1
#define LED_OFF 0

#define LED1 BIT0
#define LED2 BIT1
#define LED_ALL ( BIT1 | BIT0 )

extern void halLed_initialize();
void halLed_setLed(char leds, char flag);

#endif
