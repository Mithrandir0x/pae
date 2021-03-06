/**
 * @file hal_buttons.h
 */

#ifndef _HAL_BUTTONS_H_
#define _HAL_BUTTONS_H_

#include <msp430x54xa.h>

#define BUTTON_S1  BIT6 // P2.6
#define BUTTON_S2  BIT7 // P2.7
#define BUTTON_ALL ( BIT7 | BIT6 )

#define JOYSTICK_LEFT   BIT1 // P2.1
#define JOYSTICK_RIGHT  BIT2 // P2.2
#define JOYSTICK_CENTER BIT3 // P2.3
#define JOYSTICK_UP     BIT4 // P2.4
#define JOYSTICK_DOWN   BIT5 // P2.5
#define JOYSTICK_ALL    ( BIT5 | BIT4 | BIT3 | BIT2 | BIT1 )

void halButtons_initialize();
void halButtons_shutdown();
void halButtons_setInterruptions(int bits, int flag);
void halButtons_toggleInterruptions(int bits);

void halJoystick_initialize();
void halJoystick_shutdown();
void halJoystick_setInterruptions(int bits, int flag);
void halJoystick_toggleInterruptions(int bits);

#endif
