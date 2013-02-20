/**
 * Implementation of low-level API for MSP430F5438's LEDs.
 *
 * @author olopezsa13
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
 * @param flag A boolean flag to indicate whether the LED
 *             should be enabled or disabled.
 */
void halLed_setLed1(int flag)
{
	if ( flag == 0 )
		P1OUT &= 0xFE; // Disable the LED // MASK = 0xFF - BIT0
	else
		P1OUT |= BIT0; // Enable the LED
}


/**
 * Set the state of the LED2, either ON or OFF.
 *
 * @param flag A boolean flag to indicate whether the LED
 *             should be enabled or disabled.
 */
void halLed_setLed2(int flag)
{
	if ( flag == 0 )
		P1OUT &= 0xFD; // Disable the LED // MASK = 0xFF - BIT1
	else
		P1OUT |= BIT1; // Enable the LED
}
