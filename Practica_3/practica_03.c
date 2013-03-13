#include <msp430x54xa.h>
#include <stdio.h>

#include <hal_common.h>
#include <hal_buttons.h>
#include <hal_lcd.h>
#include <hal_led.h>
#include <hal_timer.h>

char lcd_line[17] = "PRACTICA 3";
char lcd_clear[]  = "                 ";

unsigned char lcd_contrast  = 0x64;
unsigned char lcd_backlight = 30;

unsigned char bitled;
unsigned char update_time = 0;
unsigned char update_leds = 0;
unsigned int time = 1;
unsigned int timer_multiplier = 1000;
unsigned int old_timer_multiplier = 0;

typedef struct {
    int hours;
    int minutes;
    int seconds;
} TIME;
TIME cron;

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
    halTimer_b_setCCRTimedInterruption(TIMER_CCR0, time * timer_multiplier);
    halTimer_b_setCCRTimedInterruption(TIMER_CCR1, 1000);
   // halTimer_b_setInterruptions(ON);
    halTimer_b_setCCRInterruption(TIMER_CCR0, ON);
    halTimer_b_setCCRInterruption(TIMER_CCR1, ON);
}

int get_year()
{
    return ( RTCYEARL | ( RTCYEARH << 8 ) );
}

void main()
{
    cron.seconds = 0;
    cron.minutes = 0;
    cron.hours = 0;

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer (good dog)

    _disable_interrupt();

    initialize_leds();
    initialize_lcd();
    initialize_buttons();
    initialize_timerb();

    _enable_interrupt();

    halLcdPrintLine(lcd_line, 0, OVERWRITE_TEXT);

    sprintf(lcd_line, " MUL: %05u", timer_multiplier);
    halLcdPrintLine(lcd_line, 1, OVERWRITE_TEXT);

    while ( 1 );
}

#pragma vector = PORT2_VECTOR
__interrupt void on_button_interruption(void)
{
    halJoystick_setInterruptions((JOYSTICK_UP | JOYSTICK_DOWN), OFF);

    switch ( P2IFG )
    {
        case JOYSTICK_UP:
            if ( timer_multiplier < 10000 )
                timer_multiplier *= 10;
            break;
        case JOYSTICK_DOWN:
            if ( timer_multiplier > 1 )
                timer_multiplier /= 10;
            break;
    }

    sprintf(lcd_line, " SEC: %05u", time * timer_multiplier);
    halLcdPrintLine(lcd_line, 1, OVERWRITE_TEXT);

    halTimer_b_setCCRTimedInterruption(TIMER_CCR0, time * timer_multiplier);

    P2IFG = 0;

    halJoystick_setInterruptions((JOYSTICK_UP | JOYSTICK_DOWN), ON);
}

#pragma vector = TIMERB0_VECTOR
__interrupt void on_timer_b_interruption()
{
    halTimer_b_setCCRInterruption(TIMER_CCR0, OFF);

    halLed_sx_toggleLed(LED_SX_ALL);

    halLed_rx_setLed(LED_RX_ALL, OFF);
    halLed_rx_setLed(bitled, ON);

    if ( bitled == LED_R8 )
        bitled = BIT0;
    else
        bitled <<= 1;

    halTimer_b_setCCRInterruption(TIMER_CCR0, ON);
}

#pragma vector = TIMERB1_VECTOR
__interrupt void update_hour()
{
    halTimer_b_setCCRInterruption(TIMER_CCR1, OFF);

    //if ( TB0IV == TIMER_CCR1 )
    if ( ( TB0CCR1 & TIMER_CCR_CCIFG ) != 0 )
    {
        cron.seconds++;

        if ( cron.seconds == 60 )
        {
            cron.seconds = 0;
            cron.minutes++;
        }

        if ( cron.minutes == 60 )
        {
            cron.minutes = 0;
            cron.hours++;
        }

        sprintf(lcd_line, " T: %02d %02d %02d", cron.hours, cron.minutes, cron.seconds);
        halLcdPrintLine(lcd_line, 6, OVERWRITE_TEXT);
    }

    halTimer_b_setCCRInterruption(TIMER_CCR1, ON);
}
