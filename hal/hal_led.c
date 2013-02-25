/**
 * Implementation of low-level API for MSP430F5438A's LEDs.
 *
 * @file hal_led.c
 */

#include <msp430x54xa.h>
#include "hal_led.h"

/**
 * Initialize F5438 registers to allow the LEDs S1 and S2 to work.
 */
void halLed_sx_initialize()
{
	P1DIR |= LED_SX_ALL; //Port lines P1.0 y P1.1 as OUTPUT
}

/**
 * Set the state of the LED1, either ON or OFF.
 *
 * @param leds An integer to indicate which leds should be enabled.
 * @param flag A boolean flag to indicate whether the LED should be enabled or disabled.
 */
void halLed_sx_setLed(char leds, char flag)
{
	if ( flag == 0 )
		P1OUT &= ~( leds & LED_SX_ALL ); // Disable the LED // MASK = 0xFF - BIT0
	else
		P1OUT |= ( leds & LED_SX_ALL ); // Enable the LEDs

	// Why the "& LED_SX_ALL"?
	// Protect the other bits from being tampered.
}
