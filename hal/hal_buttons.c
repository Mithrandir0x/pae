/**
 * Implementation of low-level API for MSP430F5438A's S1 and S2 buttons,
 * and the JOYSTICK.
 *
 * @file hal_buttons.c
 */

#include <msp430x54xa.h>
#include "hal_buttons.h"

/**
 * Initialize registers to enable the buttons.
 */
void halButtons_initialize()
{
	P2DIR &= ~BUTTON_ALL; // Ports P2.6 and P2.7 as "INPUT" (S1 and S2, respectively)
	P2SEL &= ~BUTTON_ALL; // Ports P2.6 and P2.7 as "General-purpose Digital I/O"
	P2REN |=  BUTTON_ALL; // Enable P2.6 and P2.7 "Pull-up/Pull-down resistor" ( http://en.wikipedia.org/wiki/Pull-up_resistor )
	//P2IE  |=  BUTTON_ALL; // Enable interruptions for P2.6 and P2.7. (Should not enable ints when initializing the device...)
	P2OUT |=  BUTTON_ALL; // Set initial state as "Pull-up"
	P2IES &= ~BUTTON_ALL; // Set interrupt edge to Low-to-High transition ( http://en.wikipedia.org/wiki/Interrupt#Edge-triggered )
}

void halButtons_setInterruptions(char bits, char flag)
{
    if ( flag == 0 )
        P2IE &= ~( bits & BUTTON_ALL );
    else
        P2IE |= ( bits & BUTTON_ALL );
}

void halButtons_toggleInterruptions(char bits)
{
    P2IE ^= ( bits & BUTTON_ALL );
}

/**
 * Initialize registers to enable the joystick.
 */
void halJoystick_initialize()
{
    P2DIR &= ~JOYSTICK_ALL; // Ports P2.1 to P2.5 as "INPUT" (S1 and S2, respectively)
    P2SEL &= ~JOYSTICK_ALL; // Ports P2.1 to P2.5 as "General-purpose Digital I/O"
    P2REN |=  JOYSTICK_ALL; // Enable P2.1 to P2.5 "Pull-up/Pull-down resistor" ( http://en.wikipedia.org/wiki/Pull-up_resistor )
    //P2IE  |=  JOYSTICK_ALL; // Enable interruptions for P2.1 to P2.5.
    P2OUT |=  JOYSTICK_ALL; // Set initial state as "Pull-up"
    P2IES &= ~JOYSTICK_ALL; // Set interrupt edge to Low-to-High transition ( http://en.wikipedia.org/wiki/Interrupt#Edge-triggered )
}

void halJoystick_setInterruptions(char bits, char flag)
{
    if ( flag == 0 )
        P2IE &= ~( bits & JOYSTICK_ALL );
    else
        P2IE |= ( bits & JOYSTICK_ALL );
}

void halJoystick_toggleInterruptions(char bits)
{
    P2IE ^= ( bits & JOYSTICK_ALL );
}
