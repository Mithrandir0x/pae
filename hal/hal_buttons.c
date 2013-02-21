/**
 * Implementation of low-level API for MSP430F5438A's S1 and S2 buttons.
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
	P2DIR &= ~( BIT7 | BIT6 ); // Ports P2.6 and P2.7 as "INPUT" (S1 and S2, respectively)
	P2SEL &= ~( BIT7 | BIT6 ); // Ports P2.6 and P2.7 as "General-purpose Digital I/O"
	P2REN |=  ( BIT7 | BIT6 ); // Enable P2.6 and P2.7 "Pull-up/Pull-down resistor" ( http://en.wikipedia.org/wiki/Pull-up_resistor )
	P2IE  |=  ( BIT7 | BIT6 ); // Enable interruptions for P2.6 and P2.7.
	P2OUT |=  ( BIT7 | BIT6 ); // Set initial state as "Pull-up"
	P2IES &= ~( BIT7 | BIT6 ); // Set interrupt edge to Low-to-High transition ( http://en.wikipedia.org/wiki/Interrupt#Edge-triggered )
}
