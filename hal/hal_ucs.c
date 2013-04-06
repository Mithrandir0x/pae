/*
 * @file hal_ucs.c
 */

#include <msp430x54xa.h>
#include "hal_common.h"
#include "hal_ucs.h"

#define CLK_MASK ( BIT2 | BIT1 | BIT0 )

#define FLLN_16M    489

/**
 * Allows to enable or disable outputting XXCLK signal to P11.
 *
 * @param mode Either set ON or OFF to enable or disable CLK Output Signal
 */
void halUCS_setFrequencyDiagnosis(int mode)
{
    if ( mode == OFF )
    {
        P11DIR &= ~CLK_MASK; // Sets P11 as "INPUT"
        P11SEL &= ~CLK_MASK; // Sets P11 as GPIO
    }
    else
    {
        P11DIR |= CLK_MASK; // Sets P11 as "OUTPUT"
        P11SEL |= CLK_MASK; // Selects P11 to work for specific purpose (output MCLK, ACLK, SMCLK signals)
    }
}

/**
 * This is the default mode.
 *
 * Configures clock signals:
 *   - MCLK:  ~1MHz, sourced from DCO
 *   - SMCLK: ~1MHz, sourced from DCO
 *   - ACLK:  32768Hz, sourced from XT1 Osc.
 *
 * Frequency Mode: UCS_MODE_FACTORY
 */
void halUCS_setFactoryFrequency()
{
    __bis_SR_register(SCG0); // Disable the Frequency Locked Loop

    // DCO = 0, MOD = 0
    // Frequency range is divided by 32 steps. Each step is selected by DCO (5 bits)
    // Setting MOD to any value different to 0, it will change DCO value to {DCO + 1}
    UCSCTL0 = 0;

    UCSCTL1 = DCORSEL_2; // Set the frequency range to [~0.5, ~5] MHz

    UCSCTL2 = FLLD_1 + 31; // Set D value to 2 and N to 31
    UCSCTL3 = SELREF__XT1CLK + FLLREFDIV_0; // Select XT1CLK as clock reference and divide it by 1.

    // Clock signal output:
    //   2 * ( 31 + 1 ) * ( 32768 / 1 ) = 2.097.152 Hz

    UCSCTL4 = SELA__XT1CLK     // Alternative Clock source comes from XT1 Oscillator
            | SELS__DCOCLKDIV  // Subsystem Master Clock source comes from Digital-Controlled Oscillator (Divided)
            | SELM__DCOCLKDIV; // Master Clock source comes from Digital-Controlled Oscillator (Divided)

    // Here we divide the frequency value by a specific value:
    UCSCTL5 = DIVA_0 // Selects ACLK source divider to 1
            | DIVS_0 // Selects SMCLK source divider to 1
            | DIVM_0; // Selects MCLK source divider to 1

    __bic_SR_register(SCG0);  // Enable the Frequency Locked Loop
}

/**
 * Configures clock signals:
 *   - MCLK:  ~16MHz, sourced from DCO
 *   - SMCLK: ~16MHz, sourced from DCO
 *   - ACLK:  32768Hz, sourced from XT1 Osc.
 *
 * Frequency Mode: UCS_MODE_16M
 */
void halUCS_set16MFrequency()
{
    __bis_SR_register(SCG0); // Disable the Frequency Locked Loop
                             // More information at page 30 (Operating Modes)

    // DCO = 0, MOD = 0
    // Frequency range is divided by 32 steps. Each step is selected by DCO (5 bits)
    // Setting MOD to any value different to 0, it will change DCO value to {DCO + 1}
    UCSCTL0 = 0;

    UCSCTL1 = DCORSEL_7; // Set the frequency range to [~13, ~100] MHz (Page 50 MSP430F5438A Datasheet)

    UCSCTL2 = FLLN_16M + FLLD_0; // Set D value to 1 and N to 489
    UCSCTL3 = SELREF__XT1CLK + FLLREFDIV_0; // Select XT1CLK as clock reference and divide it by 1.

    // Up to here, we have set the clock to be issuing a signal of ~16Mhz.
    // Exactly, this would be:
    //   1 * ( 489 + 1 ) * ( 32768 / 1 ) = 16.056.320 Hz

    UCSCTL4 = SELA__XT1CLK  // Alternative Clock source comes from XT1 Oscillator
            | SELS__DCOCLK  // Subsystem Master Clock source comes from Digital-Controlled Oscillator
            | SELM__DCOCLK; // Master Clock source comes from Digital-Controlled Oscillator

    // Here we divide the frequency value by a specific value:
    UCSCTL5 = DIVA_0 // Selects ACLK source divider to 1
            | DIVS_0 // Selects SMCLK source divider to 2
            | DIVM_0; // Selects MCLK source divider to 1

    __bic_SR_register(SCG0);  // Enable the Frequency Locked Loop
}


