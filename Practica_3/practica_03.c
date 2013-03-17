#include <msp430x54xa.h>
#include <stdio.h>

#include <hal_common.h>
#include <hal_buttons.h>
#include <hal_lcd.h>
#include <hal_led.h>
#include <hal_timer.h>

#define EDIT_HOURS   1
#define EDIT_MINUTES 2
#define EDIT_SECONDS 3

#define LINE_TIMER_SECONDS 1
#define LINE_ALARM 3
#define LINE_CRON 5
#define LINE_TIME_UNIT_SEL 6

char lcd_line[17] = "PRACTICA 3";
char lcd_clear[]  = "                 ";

unsigned char lcd_contrast  = 0x64;
unsigned char lcd_backlight = 30;

unsigned char bitled = LED_R1;

int disabled_alarm = 0;

int stop_cron = 0;

int edit_mode = OFF;
int old_edit_mode = OFF;
unsigned int tag_column = 0;

unsigned int time_multiplier = 1;
unsigned int time_base = 1000;
typedef struct {
    int hours;
    int minutes;
    int seconds;
} TIME;
TIME cron;
TIME alarm;

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
    halJoystick_setInterruptions(JOYSTICK_ALL, ON);

    halButtons_initialize();
    halButtons_setInterruptions(BUTTON_ALL, ON);
}

void initialize_timer_b()
{
    halTimer_b_initialize(TIMER_CLKSRC_ACLK, TIMER_MODE_UP);
    halTimer_b_setCCRTimedInterruption(TIMER_CCR0, time_multiplier * time_base);
    halTimer_b_enableInterruptCCR0();
}

void initialize_timer_a1()
{
    halTimer_a1_initialize(TIMER_CLKSRC_ACLK, TIMER_MODE_UP);
    halTimer_a1_setCCRTimedInterruption(TIMER_CCR0, 1000);
    halTimer_a1_enableInterruptCCR0();
}

void write_time_base()
{
    sprintf(lcd_line, " SEC: %07u", time_multiplier * time_base);
    halLcdPrintLine(lcd_line, LINE_TIMER_SECONDS, OVERWRITE_TEXT);
}

void write_cron()
{
    sprintf(lcd_line, " %02d:%02d:%02d", cron.hours, cron.minutes, cron.seconds);
    halLcdPrintLine(lcd_line, LINE_CRON, OVERWRITE_TEXT);
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer (good dog)

    _disable_interrupt();

    cron.seconds = 0;
    cron.minutes = 0;
    cron.hours = 0;

    alarm.seconds = 30;
    alarm.minutes = 2;
    alarm.hours = 0;

    initialize_leds();
    initialize_lcd();
    initialize_buttons();
    initialize_timer_b();
    initialize_timer_a1();

    _enable_interrupt();

    halLcdPrintLine(lcd_line, 0, OVERWRITE_TEXT);

    write_time_base();
    write_cron();

    while ( 1 );
}

inline void increase_cron_unit()
{
    switch ( edit_mode )
    {
        case EDIT_SECONDS:
            cron.seconds++;
            if ( cron.seconds == 60 )
                cron.seconds = 0;
            else
                break;
        case EDIT_MINUTES:
            cron.minutes++;
            if ( cron.minutes == 60 )
                cron.minutes = 0;
            else
                break;
        case EDIT_HOURS:
            cron.hours++;
            if ( cron.hours == 24 )
                cron.hours = 0;
            break;
    }
}

inline void decrease_cron_unit()
{
    switch ( edit_mode )
    {
        case EDIT_HOURS:
            if ( cron.hours > 0 )
                cron.hours--;
            break;
        case EDIT_MINUTES:
            if ( cron.minutes > 0 )
                cron.minutes--;
            break;
        case EDIT_SECONDS:
            if ( cron.seconds > 0 )
                cron.seconds--;
            break;
    }
}

#pragma vector = PORT2_VECTOR
__interrupt void on_button_interruption(void)
{
    halButtons_setInterruptions(BUTTON_ALL, OFF);
    halJoystick_setInterruptions(JOYSTICK_ALL, OFF);

    switch ( P2IFG )
    {
        case JOYSTICK_RIGHT:
            edit_mode++;
            if ( edit_mode > EDIT_SECONDS )
                edit_mode = OFF;
            break;
        case JOYSTICK_LEFT:
            edit_mode--;
            if ( edit_mode < 0 )
            	edit_mode = EDIT_SECONDS;
            break;
        case JOYSTICK_UP:
            if ( time_base < 10000 )
                time_base *= 10;
            break;
        case JOYSTICK_DOWN:
            if ( time_base > 1 )
                time_base /= 10;
            break;
        case JOYSTICK_CENTER:
            stop_cron = ~stop_cron;
            break;
        case BUTTON_S1:
            increase_cron_unit();
            break;
        case BUTTON_S2:
            decrease_cron_unit();
            break;
    }

    write_time_base();

    halTimer_b_setCCRTimedInterruption(TIMER_CCR0, time_multiplier * time_base);

    P2IFG = 0;

    halButtons_setInterruptions(BUTTON_ALL, ON);
    halJoystick_setInterruptions(JOYSTICK_ALL, ON);
}

#pragma vector = TIMER_A1_CCR0_VECTOR // TIMER1_A0_VECTOR
__interrupt void update_cron()
{
    halTimer_a1_disableInterruptCCR0();

    // Update the chronometer
    {
        if ( !stop_cron )
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

            if ( cron.hours == 24 )
                cron.hours = 0;

            write_cron();
        }
    }

    // Update alarm! ACHTUNG!
    {
        if ( !disabled_alarm && cron.seconds == alarm.seconds
                && cron.minutes == alarm.minutes && cron.hours == alarm.hours )
        {
            halLcdPrintLine("ALARM ALARM ALARM", LINE_ALARM, OVERWRITE_TEXT);
            disabled_alarm = -1;
        }
    }

    // Update time unit editor selector
    {
        if ( edit_mode != old_edit_mode )
        {
            switch ( edit_mode )
            {
                case EDIT_HOURS:
                    sprintf(lcd_line, "%2c", "HH");
                    tag_column = 1;
                    break;
                case EDIT_MINUTES:
                    sprintf(lcd_line, "%2c", "MM");
                    tag_column = 4;
                    break;
                case EDIT_SECONDS:
                    sprintf(lcd_line, "%2c", "SS");
                    tag_column = 7;
                    break;
            }

            // | 00:00:00
            // | HH MM SS
            //   1  4  7
            halLcdPrintLine(lcd_clear, LINE_TIME_UNIT_SEL, OVERWRITE_TEXT);
            halLcdPrintLineCol(lcd_line, LINE_TIME_UNIT_SEL, tag_column, OVERWRITE_TEXT);
        }

        old_edit_mode = edit_mode;
    }

    halTimer_a1_enableInterruptCCR0();
}

#pragma vector = TIMERB0_VECTOR
__interrupt void update_leds()
{
    halTimer_b_disableInterruptCCR0();

    halLed_sx_toggleLed(LED_SX_ALL);

    halLed_rx_setLed(LED_RX_ALL, OFF);
    halLed_rx_setLed(bitled, ON);

    if ( bitled == LED_R8 )
        bitled = BIT0;
    else
        bitled <<= 1;

    halTimer_b_enableInterruptCCR0();
}
