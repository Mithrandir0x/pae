/*
 * @file hal_rtc.c
 */

#include "hal_common.h"
#include "hal_rtc.h"

void halRTC_initialize(int mode, int source, int format, int enableInterruptions)
{
    RTCCTL01 |= ( source & CCTL1_RTCSSEL );
    RTCCTL01 |= ( format & CCTL1_RTCBCD );

    if ( enableInterruptions == OFF )
        RTCCTL01 &= ~RTC_RTCRDYIE;
    else
        RTCCTL01 |= RTC_RTCRDYIE;

    RTCCTL01 |= ( mode   & CCTL1_RTCMODE );
}

void halRTC_setMode(int mode)
{
    RTCCTL1 &= ~CCTL1_RTCMODE;
    RTCCTL1 |= ( mode & CCTL1_RTCMODE );
}

void halRTC_setClockSource(int source)
{
    RTCCTL1 &= ~CCTL1_RTCSSEL;
    RTCCTL1 |= ( source & CCTL1_RTCSSEL );
}

void halRTC_setRegisterFormat(int format)
{
    RTCCTL1 &= ~CCTL1_RTCBCD;
    RTCCTL1 |= ( format & CCTL1_RTCBCD );
}

void halRTC_setInterruptions(int boolean)
{
    if ( boolean == OFF )
        RTCCTL0 &= ~RTCRDYIE;
    else
        RTCCTL0 |= RTCRDYIE;
}
