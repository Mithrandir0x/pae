/*
 * @file hal_timer.c
 */

#include <msp430x54xa.h>

#include "hal_common.h"
#include "hal_timer.h"

// For more oscillator information, refer to:
//   MSP430F5438_Datasheet, Crystal Oscillator XT1, Low-Frequency Mode (page 46)

#define ACLK_TICKS_PER_SECOND 32 // floor(32768 Hz / 1000 ms)
#define SMCLK_TICK_PER_SECOND 1000 // 10 ** 6 Hz / 10 ** 3 ms

void halTimer_b_initialize(int source, int mode)
{
    TB0CTL = 0;
    TB0CTL |= ( source & TBSSEL );
    TB0CTL |= ( mode & MC );
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
	volatile unsigned int *TB0CCTLX = NULL;

	switch ( ccr )
	{
		case TIMER_B_CCR0:
			TB0CCTLX = &TB0CCTL0;
			break;
		case TIMER_B_CCR1:
			TB0CCTLX = &TB0CCTL1;
			break;
		case TIMER_B_CCR2:
			TB0CCTLX = &TB0CCTL2;
			break;
		case TIMER_B_CCR3:
			TB0CCTLX = &TB0CCTL3;
			break;
		case TIMER_B_CCR4:
			TB0CCTLX = &TB0CCTL4;
			break;
		case TIMER_B_CCR5:
			TB0CCTLX = &TB0CCTL5;
			break;
		case TIMER_B_CCR6:
			TB0CCTLX = &TB0CCTL6;
			break;
	}

	if ( TB0CCTLX != NULL )
	{
	    if ( boolean == OFF )
	    	*TB0CCTLX &= ~CCIFG;
	    else
	    	*TB0CCTLX |= CCIFG;
	}
}

/**
 * @param ccr Select which of the CCR do we use to compare. Remember that CCR0 has the utmost priority.
 * @param time Number of milliseconds to wait before interruption.
 */
void halTimer_b_setCCRTimedInterruption(int ccr, unsigned int time)
{
    unsigned int ticks = 0;

    // Check which clock are we using, and calculate the
    // number of ticks required for the amount of time
    switch (TBSSEL)
    {
        case TIMER_CLKSRC_ACLK:
            ticks = ACLK_TICKS_PER_SECOND * time;
            break;

        case TIMER_CLKSRC_SMCLK:
            ticks = SMCLK_TICK_PER_SECOND * time;
            break;
    }

    // Save the value to the selected register
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
