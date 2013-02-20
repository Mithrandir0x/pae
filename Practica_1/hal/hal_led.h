/**
 * @author olopezsa13
 * @file hal_led.h
 */

#ifndef _HAL_LED_
#define _HAL_LED_

#define LED_ON  1
#define LED_OFF 0

extern void halLed_initialize();
extern void halLed_setLed1(int flag);
extern void halLed_setLed2(int flag);

#endif
