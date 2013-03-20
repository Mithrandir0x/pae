/*
 * @file hal_timer.h
 */

// Diapo 17, flag CCIFG TA0CCTL0

#ifndef HAL_TIMER_H_
#define HAL_TIMER_H_

#include <msp430x54xa.h>

#define TIMER_CNT_16 0                 // 0xFFFF
#define TIMER_CNT_12 BIT11             // 0xFFF
#define TIMER_CNT_10 BIT12             // 0x3FF
#define TIMER_CNT_8  ( BIT12 | BIT11 ) // 0xFF

#define TIMER_CLKSRC_ACLK  BIT8 // 32768 Hz == 2**15 Hz // Alternative Clock
#define TIMER_CLKSRC_SMCLK BIT9 // 1 Mhz // Secondary Master Clock

#define TIMER_MODE_STOP       0
#define TIMER_MODE_UP         BIT4
#define TIMER_MODE_CONTINUOUS BIT5
#define TIMER_MODE_UPDOWN     ( BIT5 | BIT4 )

#define TIMER_CCR0    0  // 0000
#define TIMER_CCR1    2  // 0010
#define TIMER_CCR2    4  // 0100
#define TIMER_CCR3    6  // 0110
#define TIMER_CCR4    8  // 1000
#define TIMER_CCR5    10 // 1010
#define TIMER_CCR6    12 // 1100

#define TIMER_A1_CCR0_VECTOR TIMER1_A0_VECTOR
#define TIMER_A1_CCRX_VECTOR TIMER1_A1_VECTOR

void halTimer_a1_initialize(int source, int mode);
//void halTimer_a1_setClockSource(int source);
//void halTimer_a1_setMode(int mode);
//void halTimer_a1_setInterruptions(int boolean);
//void halTimer_a1_setCCRInterruption(int ccr, int boolean);
void halTimer_a1_setCCRTimedInterruption(int ccr, unsigned int time);
//void halTimer_a1_clear();

void halTimer_b_initialize(int source, int mode);
//void halTimer_b_setClockSource(int source);
//void halTimer_b_setMode(int mode);
//void halTimer_b_setInterruptions(int boolean);
//void halTimer_b_setCCRInterruption(int ccr, int boolean);
void halTimer_b_setCCRTimedInterruption(int ccr, unsigned int time);
//void halTimer_b_clear();

inline void halTimer_b_disableInterruptCCR0()
{
	TB0CCTL0 &= ~BIT4;
}

inline void halTimer_b_enableInterruptCCR0()
{
	TB0CCTL0 |= BIT4;
}

inline void halTimer_a1_disableInterruptCCR0()
{
	TA1CCTL0 &= ~BIT4;
}

inline void halTimer_a1_enableInterruptCCR0()
{
	TA1CCTL0 |= BIT4;
}

#endif
