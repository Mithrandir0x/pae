/*
 * @file hal_timer.c
 */

#include <msp430x54xa.h>

#include "hal_common.h"
#include "hal_timer.h"

void halTimer_b_initialize()
{
    TB0CTL = 0;
}

/**
 * @param source The source to get pulses from.
 */
void halTimer_b_setClockSource(int source)
{
    TB0CTL |= ( source & TBSSEL );
}

/**
 * @param mode The mode to be set in TIMER_B.
 */
void halTimer_b_setMode(int mode)
{
    TB0CTL |= ( mode & MC );
}

/**
 */
void halTimer_b_clear()
{
    TB0CTL &= ~TBCLR;
}

/**
 * @param boolean Either set interruptions ON or OFF.
 */
void halTimer_b_setInterruptions(int boolean)
{
    if ( flag == OFF )
        TB0CTL &= ~TBIE;
    else
        TB0CTL |= TBIE;
}

void halTimer_b_setTimedInterruption(int ccr, unsigned int time)
{
    unsigned int ticks = 0;

    // Averiguar el reloj que estamos usando.
    switch (TBSSEL)
    {
        case TIMER_CLKSRC_ACLK:
            ticks = (32768 / 1000)*time;
            break;

        case TIMER_CLKSRC_SMCLK:
            ticks = 1000*time;
            break;
    }

    // Calculamos el numero de ticks respecto a ese valor, y lo guardamos
    switch (ccr)
    {
        case TIMER_B_CCR0:
            TB0CCR0 = ticks;
            break;
        case TIMER_B_CCR1:
            TB0CCR1 = ticks;
            break;
        case TIMER_B_CCR2:
            TB0CCR2 = ticks;
            break;
        case TIMER_B_CCR3:
            TB0CCR3 = ticks;
            break;
        case TIMER_B_CCR4:
            TB0CCR4 = ticks;
            break;
        case TIMER_B_CCR5:
            TB0CCR5 = ticks;
            break;
        case TIMER_B_CCR6:
            TB0CCR6 = ticks;
            break;
    }
}
