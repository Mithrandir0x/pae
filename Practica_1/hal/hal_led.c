/**
 * Implementation of low-level API for MSP430F5438A's LEDs.
 *
 * @file hal_led.c
 */

#include <msp430x54xa.h>
#include "hal_led.h"

/**
 * Initialize F5438 registers to allow the LEDs to work.
 */
void halLed_initialize()
{
	P1DIR |= ( BIT0 | BIT1 ); //Port lines P1.0 y P1.1 as OUTPUT
}

/**
 * Set the state of the LED1, either ON or OFF.
 *
 * @param leds An integer to indicate which leds should be enabled.
 * @param flag A boolean flag to indicate whether the LED should be enabled or disabled.
 */
void halLed_setLeds(char leds, char flag)
{
	if ( flag == 0 )
		P1OUT &= 0xFF & ~( leds & ( BIT0 | BIT1 ) ); // Disable the LED // MASK = 0xFF - BIT0
	else
		P1OUT |= ( leds & ( BIT0 | BIT1 ) ); // Enable the LEDs

	// Why the "& ( BIT0 | BIT1 )"?
	// Protect the other bits from being tampered.
}
