/**
 * @file hal_buttons.h
 */

#include <msp430x54xa.h>

#ifndef _HAL_BUTTONS_H_
#define _HAL_BUTTONS_H_

#define BUTTON_S1  BIT6
#define BUTTON_S2  BIT7
#define BUTTON_ALL ( BIT7 | BIT6 )

#define JOYSTICK_LEFT   BIT1
#define JOYSTICK_RIGHT  BIT2
#define JOYSTICK_CENTER BIT3
#define JOYSTICK_UP     BIT4
#define JOYSTICK_DOWN   BIT5
#define JOYSTICK_ALL    ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 )

void halButtons_initialize();
void halJoystick_initialize();

#endif
