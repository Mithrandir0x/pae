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

#define TBCNTL BIT12 | BIT11
#define TBSSEL BIT9 | BIT8
#define MC     BIT5 | BIT4
#ifndef TBCLR
#define TBCLR  BIT2
#endif
#ifndef TBIE
#define TBIE   BIT1
#endif
#ifndef CCIFG
#define CCIFG BIT0
#endif
#define TIMER_CCR_CCIE BIT4

void halTimer_b_initialize(int source, int mode);
void halTimer_b_setClockSource(int source);
void halTimer_b_setMode(int mode);
void halTimer_b_setInterruptions(int boolean);
void halTimer_b_setCCRInterruption(int ccr, int boolean);
void halTimer_b_setCCRTimedInterruption(int ccr, unsigned int time);
void halTimer_b_clear();

#endif
