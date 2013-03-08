/**
 * @file hal_rtc.h
 */

#ifndef HAL_RTC_H_
#define HAL_RTC_H_

#include <msp430x54xa.h>

#define RTC_COUNTER_MODE  0
#define RTC_CALENDAR_MODE BIT5

#define RTC_CLKSRC_ACLK  0
#define RTC_CLKSRC_SMCLK BIT2

#define RTC_REGM_BINHEX 0
#define RTC_REGM_BCD    BIT7

#define CCTL1_RTCMODE BIT5
#define CCTL1_RTCSSEL ( BIT3 | BIT2 )
#define CCTL1_RTCBCD  BIT7

void halRTC_initialize(int mode, int source, int format, int enableInterruptions);

void halRTC_setMode(int mode);
void halRTC_setClockSource(int source);
void halRTC_setRegisterFormat(int format);
void halRTC_setInterruptions(int boolean);

#endif
