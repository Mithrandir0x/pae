/*
 * @file hal_timer.h
 */

// Diapo 17, flag CCIFG TA0CCTL0

#ifndef HAL_TIMER_H_
#define HAL_TIMER_H_

#include <msp430x54xa.h>

#define TIMER_CNT_16 0 // 0xFFFF
#define TIMER_CNT_12 1 // 0xFFF
#define TIMER_CNT_10 2 // 0x3FF
#define TIMER_CNT_8  3 // 0xFF

#define TIMER_CLKSRC_ACLK  1 // 32768 Hz == 2**15 Hz
#define TIMER_CLKSRC_SMCLK 2 // 1 Mhz

#define TIMER_MODE_STOP       0
#define TIMER_MODE_UP         1
#define TIMER_MODE_CONTINUOUS 2
#define TIMER_MODE_UPDOWN     3

#define TIMER_B_CCR0    0
#define TIMER_B_CCR1    2
#define TIMER_B_CCR2    4
#define TIMER_B_CCR3    6
#define TIMER_B_CCR4    8
#define TIMER_B_CCR5    10
#define TIMER_B_CCR6    12

#define TBCNTL BIT12 | BIT11
#define TBSSEL BIT9 | BIT8
#define MC     BIT5 | BIT4
#ifndef TBCLR
#define TBCLR  BIT2
#endif
#ifndef TBIE
#define TBIE   BIT1
#endif

void halTimer_b_initialize();
void halTimer_b_setClockSource(int source);
void halTimer_b_setMode(int mode);
void halTimer_b_setInterruptions(int flag);

void halTimer_b_setTimedInterruption(int ccr, unsigned long int time);
void halTimer_b_clear();

#endif
