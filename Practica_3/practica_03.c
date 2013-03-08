#include <msp430x54xa.h>
#include <stdio.h>

#include <hal_common.h>
#include <hal_buttons.h>
#include <hal_lcd.h>
#include <hal_led.h>
#include <hal_rtc.h>
#include <hal_timer.h>

char lcd_line[17] = "PRACTICA 3";
char lcd_clear[]  = "                 ";

unsigned char lcd_contrast  = 0x64;
unsigned char lcd_backlight = 30;

unsigned char bitled;
unsigned char update_time = 0;
unsigned char update_leds = 0;
unsigned int time = 1;
unsigned int timer_multiplier = 1;
unsigned int old_timer_multiplier = 0;

void initialize_leds()
{
    halLed_sx_initialize();
    halLed_sx_setLed(LED_RED, ON);
    halLed_sx_setLed(LED_YELLOW, OFF);

    halLed_rx_initialize();
    halLed_rx_setLed(LED_RX_ALL, OFF);
}

void initialize_lcd()
{
    halLcdInit();
    halLcdBackLightInit();
    halLcdSetBackLight(lcd_backlight);
    halLcdSetContrast(lcd_contrast);
    halLcdClearScreen();
}

void initialize_buttons()
{
    halJoystick_initialize();
    halJoystick_setInterruptions(JOYSTICK_UP | JOYSTICK_DOWN, ON);
}

void initialize_timerb()
{
    halTimer_b_initialize(TIMER_CLKSRC_ACLK, TIMER_MODE_UP);
    halTimer_b_setCCRTimedInterruption(TIMER_B_CCR0, time * timer_multiplier);
    halTimer_b_setInterruptions(ON);
}

void initialize_rtc()
{
    halRTC_initialize(RTC_CALENDAR_MODE, RTC_CLKSRC_ACLK, RTC_REGM_BCD, TRUE);
}

int get_year()
{
    return ( RTCYEARL | ( RTCYEARH << 8 ) );
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer (good dog)

    _disable_interrupt();

    initialize_leds();
    initialize_lcd();
    initialize_buttons();
    initialize_timerb();

    _enable_interrupt();

    halLcdPrintLine(lcd_line, 0, OVERWRITE_TEXT);

    sprintf(lcd_line, " %02d/%02d/%04d", RTCDAY, RTCMON, get_year());
    halLcdPrintLine(lcd_line, 2, OVERWRITE_TEXT);

    do
    {
        if ( old_timer_multiplier != timer_multiplier )
        {
            sprintf(lcd_line, "  MUL: %05d", timer_multiplier);
            halLcdPrintLine(lcd_line, 0, OVERWRITE_TEXT);

            old_timer_multiplier = timer_multiplier;
        }

        if ( update_time )
        {
            sprintf(lcd_line, " %02d:%02d:%02d", RTCHOUR, RTCMIN, RTCSEC);
            halLcdPrintLine(lcd_line, 3, OVERWRITE_TEXT);

            update_time = 0;
        }

        if ( update_leds )
        {
            halLed_sx_toggleLed(LED_SX_ALL);

            halLed_rx_setLed(LED_RX_ALL, OFF);
            halLed_rx_setLed(bitled, ON);

            if ( bitled == LED_R8 )
                bitled = BIT0;
            else
                bitled <<= 1;

            update_leds = 0;
        }
    }
    while ( 1 );
}

#pragma vector = PORT2_VECTOR
__interrupt void on_button_interruption(void)
{
    //halJoystick_setInterruptions(JOY_UPDOWN, OFF);

    switch ( P2IFG )
    {
        case JOYSTICK_UP:
            if ( timer_multiplier < 10000 )
                timer_multiplier *= 10;
            break;
        case JOYSTICK_DOWN:
            if ( timer_multiplier != 1 )
                timer_multiplier /= 10;
            break;
    }

    P2IFG = 0;
    //halJoystick_setInterruptions(JOY_UPDOWN, ON);
}

#pragma vector = TIMERB0_VECTOR
__interrupt void on_timer_b_interruption()
{
    //halTimer_b_setCCRInterruption(TIMER_B_CCR0, OFF);

    update_leds = 1;

    //halTimer_b_setCCRInterruption(TIMER_B_CCR0, ON);
}

#pragma vector = RTC_VECTOR
__interrupt void on_rtc_interruption()
{
    // RTCRDYIFG
    update_time = 1;
}
