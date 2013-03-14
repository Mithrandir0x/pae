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

unsigned char bitled = LED_R1;

unsigned int time_multiplier = 1;
unsigned int time_base = 1000;
unsigned int old_time_base = 0;

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
    halJoystick_setInterruptions(JOYSTICK_UP | JOYSTICK_DOWN, ON);
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

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer (good dog)

    _disable_interrupt();

    alarm.seconds = 0;
	alarm.minutes = 0;
	alarm.hours = 0;

    initialize_leds();
    initialize_lcd();
    initialize_buttons();
    initialize_timer_b();
    initialize_timer_a1();

    _enable_interrupt();

    halLcdPrintLine(lcd_line, 0, OVERWRITE_TEXT);

    sprintf(lcd_line, " MUL: %05u", time_base);
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
            if ( time_base < 10000 )
                time_base *= 10;
            break;
        case JOYSTICK_DOWN:
            if ( time_base > 1 )
                time_base /= 10;
            break;
    }

    sprintf(lcd_line, " SEC: %05u", time_multiplier * time_base);
    halLcdPrintLine(lcd_line, 1, OVERWRITE_TEXT);

    halTimer_b_setCCRTimedInterruption(TIMER_CCR0, time_multiplier * time_base);

    P2IFG = 0;

    halJoystick_setInterruptions((JOYSTICK_UP | JOYSTICK_DOWN), ON);
}

#pragma vector = TIMER_A1_CCR0_VECTOR // TIMER1_A0_VECTOR
__interrupt void update_cron()
{
	halTimer_a1_disableInterruptCCR0();

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
