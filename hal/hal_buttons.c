/**
 * Implementation of low-level API for MSP430F5438A's S1 and S2 buttons,
 * and the JOYSTICK.
 *
 * @file hal_buttons.c
 */

#include "hal_buttons.h"
#include "hal_common.h"

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

void halButtons_shutdown()
{
    P2IES &= ~BUTTON_ALL;
    P2IE  &= ~BUTTON_ALL;
    P2REN &= ~BUTTON_ALL;
    P2OUT &= ~BUTTON_ALL;
}

/**
 * Set the state of button interruptions.
 *
 * @param bits Bitmask indicating which buttons to set interruptions.
 * @param flag Set the state of the interruptions, whether ON or OFF.
 */
void halButtons_setInterruptions(int bits, int flag)
{
    if ( flag == OFF )
        P2IE &= ~( bits & BUTTON_ALL );
    else
        P2IE |= ( bits & BUTTON_ALL );
}

/**
 * Toggle the state of button interruptions.
 *
 * @param bits Bitmask indicating which buttons should be toggled its state.
 */
void halButtons_toggleInterruptions(int bits)
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

void halJoystick_shutdown()
{
    P2IES &= ~JOYSTICK_ALL;
    P2IE  &= ~JOYSTICK_ALL;
    P2REN &= ~JOYSTICK_ALL;
    P2OUT &= ~JOYSTICK_ALL;
}

/**
 * Set the state of joystick's button interruptions.
 *
 * @param bits Bitmask indicating which buttons to set interruptions.
 * @param flag Set the state of the interruptions, whether ON or OFF.
 */
void halJoystick_setInterruptions(int bits, int flag)
{
    if ( flag == OFF )
        P2IE &= ~( bits & JOYSTICK_ALL );
    else
        P2IE |= ( bits & JOYSTICK_ALL );
}

/**
 * Toggle the state of joystick's button interruptions.
 *
 * @param bits Bitmask indicating which buttons should be toggled its state.
 */
void halJoystick_toggleInterruptions(int bits)
{
    P2IE ^= ( bits & JOYSTICK_ALL );
}
