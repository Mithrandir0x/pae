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

#define TIME_FACTOR_MILLI 1000
#define TIME_FACTOR_MICRO 1000000

/**
 * Calculate the frequency by its clock source.
 *
 * @param selector This value is used to know from which source does the clock come from.
 * @return The clock source frequency.
 */
unsigned long calculate_clock_frequency_by_source(int selector)
{
    unsigned long d = 1;
    unsigned long n = 0;
    unsigned long clkref = 32768;
    unsigned long clkrefdiv = 1;
    unsigned long result = 0;

    if ( selector == UCS_SELX_XT1CLK )
        return 32768;

    d = 1 << ( ( UCSCTL2 & 0x7000 ) >> 12 );
    n = UCSCTL2 & 0x03FF;
    clkrefdiv = 1 << ( UCSCTL3 & 0x7 );

    if ( selector == UCS_SELX_DCOCLK )
    {
        result = d * ( n + 1 ) * ( clkref / clkrefdiv );
        return result;
    }
    else
        return ( n + 1 ) * ( clkref / clkrefdiv );
}

/**
 * Calculate the amount of ticks to be counted.
 *
 * @param control_register A timer's control register to know which clock source is using.
 * @param time             The amount of time that a timer will be counting before sending an interruption.
 * @param timefactor       The time factor that allows set an interruption based on milliseconds (TIME_FACTOR_MILLI)
 *                         or microseconds (TIME_FACTOR_MICRO).
 * @return The value to count up to by a timer before sending an interruption.
 */
unsigned int calculate_ticks(int control_register, unsigned int time, unsigned long timefactor)
{
    unsigned int ticks;
    unsigned long tps_aclk = 0;  // Ticks Per milliSecond from ACLK
    unsigned long tps_smclk = 0; // Ticks Per milliSecond from SMCLK

    // Calculate TPS for each clock signal
    tps_aclk = calculate_clock_frequency_by_source((UCSCTL4 & 0x0700 ) >> 8);
    tps_aclk = tps_aclk / ( 1 << ( ( UCSCTL5 & 0x0700 ) >> 8 ) );
    tps_aclk = tps_aclk / timefactor;
    tps_smclk = calculate_clock_frequency_by_source((UCSCTL4 & 0x0070 ) >> 4);
    tps_smclk = tps_smclk / ( 1 << ( ( UCSCTL5 & 0x0070 ) >> 4 ) );
    tps_smclk = tps_smclk / timefactor;

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
        default:
            ticks = 0xFFFF;
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
 * @param ccr Select which of the CCR do we use to compare. Remember that CCR0 has the utmost priority.
 * @param time Number of milliseconds to wait before interruption.
 */
void halTimer_a1_setTimedInterruption(int ccr, unsigned int time, unsigned long timefactor)
{
    unsigned int ticks = 0;     // Number to count before issuing an interruption

    ticks = calculate_ticks(TA1CTL, time, timefactor);

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

void halTimer_a1_setCCRTimedInterruption(int ccr, unsigned int time)
{
    halTimer_a1_setTimedInterruption(ccr, time, TIME_FACTOR_MILLI);
}

void halTimer_a1_setCCRMicroTimedInterruption(int ccr, unsigned int time)
{
    halTimer_a1_setTimedInterruption(ccr, time, TIME_FACTOR_MICRO);
}

void halTimer_b_initialize(int source, int mode)
{
    TB0CTL = 0;
    TB0CTL |= ( source & TX_CTL_TXSSEL );
    TB0CTL |= ( mode & TX_CTL_MC );
}

/**
 * @param ccr Select which of the CCR do we use to compare. Remember that CCR0 has the utmost priority.
 * @param time Number of milliseconds to wait before interruption.
 */
void halTimer_b_setTimedInterruption(int ccr, unsigned int time, unsigned long timefactor)
{
    unsigned int ticks = 0;     // Number to count before issuing an interruption

    ticks = calculate_ticks(TB0CTL, time, timefactor);

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

void halTimer_b_setCCRTimedInterruption(int ccr, unsigned int time)
{
    halTimer_b_setTimedInterruption(ccr, time, TIME_FACTOR_MILLI);
}

void halTimer_b_setCCRMicroTimedInterruption(int ccr, unsigned int time)
{
    halTimer_b_setTimedInterruption(ccr, time, TIME_FACTOR_MICRO);
}
