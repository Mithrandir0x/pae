/*
 * @file hal_timer.c
 */

#include <msp430x54xa.h>

#include "hal_common.h"
#include "hal_timer.h"

#define TX_CTL_TXSSEL ( BIT9 | BIT8 )
#define TX_CTL_MC     BIT5 | BIT4
#define TX_CCTL_CCIE  BIT4

#define TB_CTL_CNTL ( BIT12 | BIT11 )

#define TIMER_CCR_CCIFG BIT1

#ifndef TBCLR
#define TBCLR  BIT2
#endif

#ifndef TBIE
#define TBIE   BIT1
#endif

#ifndef CCIFG
#define CCIFG BIT0
#endif

#define UCS_SELX_XT1CLK    0
#define UCS_SELX_DCOCLK    3
#define UCS_SELX_DCOCLKDIV 4

/**
 * Calculate the frequency by its clock source.
 *
 * @param selector This value is used to know from which source does the clock come from.
 */
int calculate_clock_frequency_by_source(unsigned int selector)
{
    unsigned int d = 1;
    unsigned int n = 0;
    unsigned int clkref = 0;
    unsigned int clkrefdiv = 1;

    if ( selector == UCS_SELX_XT1CLK )
        return 32768;

    d = 1 << ( ( UCSCTL2 & 0x7000 ) >> 12 );
    n = UCSCTL2 & 0x03FF;
    clkrefdiv = 1 << ( UCSCTL3 & 0x7 );

    if ( selector == UCS_SELX_DCOCLK )
        return d * ( n + 1 ) * ( 32768 / clkrefdiv );
    else
        return ( n + 1 ) * ( 32768 / clkrefdiv );
}

/**
 * Calculate the amount of ticks to be counted.
 */
int calculate_ticks(int control_register, int time)
{
    unsigned int ticks = 0;     // Number to count before issuing an interruption
    unsigned int tps_aclk = 0;  // Ticks Per milliSecond from ACLK
    unsigned int tps_smclk = 0; // Ticks Per milliSecond from SMCLK

    // Calculate TPS for each clock signal
    tps_aclk = calculate_clock_frequency((UCSCTL4 & 0x0700 ) >> 8);
    tps_aclk = tps_aclk / ( 1 << ( ( UCSCTL5 & 0x0700 ) >> 8 ) );
    tps_aclk = tps_aclk / 1000;
    tps_smclk = calculate_clock_frequency((UCSCTL4 & 0x0070 ) >> 4);
    tps_smclk = tps_smclk / ( 1 << ( ( UCSCTL5 & 0x0070 ) >> 4 ) );
    tps_smclk = tps_smclk / 1000;

    // Check which clock signal the micro is using, and calculate the
    // number of ticks required for the amount of time passed by argument.
    switch ( control_register & TX_CTL_TXSSEL )
    {
        case TIMER_CLKSRC_ACLK:
            ticks = tps_aclk * time;
            break;
        case TIMER_CLKSRC_SMCLK:
            ticks = tps_smclk * time;
            break;
    }

    return ticks;
}

void halTimer_a1_initialize(int source, int mode)
{
    TA1CTL = 0;
    TA1CTL |= ( source & TX_CTL_TXSSEL );
    TA1CTL |= ( mode & TX_CTL_MC );
}

/**
 * @param source The source to get pulses from.
 */
void halTimer_a1_setClockSource(int source)
{
    TA1CTL &= ~TX_CTL_TXSSEL;
    TA1CTL |= ( source & TX_CTL_TXSSEL );
}

/**
 * @param mode The mode to be set in TIMER_B.
 */
void halTimer_a1_setMode(int mode)
{
    TA1CTL &= ~TX_CTL_MC;
    TA1CTL |= ( mode & TX_CTL_MC );
}

/**
 * Clear TIMER_B counter.
 */
void halTimer_a1_clear()
{
    TA1CTL &= ~TBCLR;
}

/**
 * @param boolean Either set TIMER_B interruptions ON or OFF.
 */
void halTimer_a1_setInterruptions(int boolean)
{
    if ( boolean == OFF )
        TA1CTL &= ~TBIE;
    else
        TA1CTL |= TBIE;
}

/**
 * @param ccr Select which of the CCR do we use to compare. Remember that CCR0 has the utmost priority.
 * @param boolean Set either ON or OFF CCR interruptions
 */
void halTimer_a1_setCCRInterruption(int ccr, int boolean)
{
    WORD_PTR TA1CCTLX = NULL;

    switch ( ccr )
    {
        case TIMER_CCR0:
            TA1CCTLX = &TA1CCTL0;
            break;
        case TIMER_CCR1:
            TA1CCTLX = &TA1CCTL1;
            break;
        case TIMER_CCR2:
            TA1CCTLX = &TA1CCTL2;
            break;
    }

    if ( TA1CCTLX != NULL )
    {
        if ( boolean == OFF )
            *TA1CCTLX &= ~TX_CCTL_CCIE;
        else
            *TA1CCTLX |= TX_CCTL_CCIE;
    }
}

/**
 * @param ccr Select which of the CCR do we use to compare. Remember that CCR0 has the utmost priority.
 * @param time Number of milliseconds to wait before interruption.
 */
void halTimer_a1_setCCRTimedInterruption(int ccr, unsigned int time)
{
    unsigned int ticks = 0;     // Number to count before issuing an interruption

    ticks = calculate_ticks(TA1CTL, time);

    // Save the value to the selected register
    switch ( ccr )
    {
        case TIMER_CCR0:
            TA1CCR0 = ticks;
            break;
        case TIMER_CCR1:
            TA1CCR1 = ticks;
            break;
        case TIMER_CCR2:
            TA1CCR2 = ticks;
            break;
    }
}

void halTimer_b_initialize(int source, int mode)
{
    TB0CTL = 0;
    TB0CTL |= ( source & TX_CTL_TXSSEL );
    TB0CTL |= ( mode & TX_CTL_MC );
}

/**
 * @param source The source to get pulses from.
 */
void halTimer_b_setClockSource(int source)
{
    TB0CTL &= ~TX_CTL_TXSSEL;
    TB0CTL |= ( source & TX_CTL_TXSSEL );
}

/**
 * @param mode The mode to be set in TIMER_B.
 */
void halTimer_b_setMode(int mode)
{
    TB0CTL &= ~TX_CTL_MC;
    TB0CTL |= ( mode & TX_CTL_MC );
}

/**
 * Clear TIMER_B counter.
 */
void halTimer_b_clear()
{
    TB0CTL &= ~TBCLR;
}

/**
 * @param boolean Either set TIMER_B interruptions ON or OFF.
 */
void halTimer_b_setInterruptions(int boolean)
{
    if ( boolean == OFF )
        TB0CTL &= ~TBIE;
    else
        TB0CTL |= TBIE;
}

/**
 * @param ccr Select which of the CCR do we use to compare. Remember that CCR0 has the utmost priority.
 * @param boolean Set either ON or OFF CCR interruptions
 */
void halTimer_b_setCCRInterruption(int ccr, int boolean)
{
    WORD_PTR TB0CCTLX = NULL;

    switch ( ccr )
    {
        case TIMER_CCR0:
            TB0CCTLX = &TB0CCTL0;
            break;
        case TIMER_CCR1:
            TB0CCTLX = &TB0CCTL1;
            break;
        case TIMER_CCR2:
            TB0CCTLX = &TB0CCTL2;
            break;
        case TIMER_CCR3:
            TB0CCTLX = &TB0CCTL3;
            break;
        case TIMER_CCR4:
            TB0CCTLX = &TB0CCTL4;
            break;
        case TIMER_CCR5:
            TB0CCTLX = &TB0CCTL5;
            break;
        case TIMER_CCR6:
            TB0CCTLX = &TB0CCTL6;
            break;
    }

    if ( TB0CCTLX != NULL )
    {
        if ( boolean == OFF )
            *TB0CCTLX &= ~TX_CCTL_CCIE;
        else
            *TB0CCTLX |= TX_CCTL_CCIE;
    }
}

/**
 * @param ccr Select which of the CCR do we use to compare. Remember that CCR0 has the utmost priority.
 * @param time Number of milliseconds to wait before interruption.
 */
void halTimer_b_setCCRTimedInterruption(int ccr, unsigned int time)
{
    unsigned int ticks = 0;     // Number to count before issuing an interruption

    ticks = calculate_ticks(TB0CTL, time);

    // Save the value to the selected register
    switch ( ccr )
    {
        case TIMER_CCR0:
            TB0CCR0 = ticks;
            break;
        case TIMER_CCR1:
            TB0CCR1 = ticks;
            break;
        case TIMER_CCR2:
            TB0CCR2 = ticks;
            break;
        case TIMER_CCR3:
            TB0CCR3 = ticks;
            break;
        case TIMER_CCR4:
            TB0CCR4 = ticks;
            break;
        case TIMER_CCR5:
            TB0CCR5 = ticks;
            break;
        case TIMER_CCR6:
            TB0CCR6 = ticks;
            break;
    }
}
