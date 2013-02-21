/**
 * @file hal_buttons.h
 */

#include <msp430x54xa.h>

#ifndef _HAL_BUTTONS_H_
#define _HAL_BUTTONS_H_

#define BUTTON_S1 BIT6
#define BUTTON_S2 BIT7

void halButtons_initialize();

#endif
