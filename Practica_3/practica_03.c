#include <msp430x54xa.h>
#include <stdio.h>

#include <hal_common.h>
#include <hal_buttons.h>
#include <hal_lcd.h>
#include <hal_led.h>
#include <hal_timer.h>

#define JOY_UPDOWN JOYSTICK_UP | JOYSTICK_DOWN

char lcd_line[17] = "PRACTICA 3";
char lcd_clear[]  = "                 ";

unsigned char lcd_contrast  = 0x64;
unsigned char lcd_backlight = 30;

unsigned char bitled;
unsigned int time = 1;
unsigned int timer_multiplier = 1;

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
    halJoystick_setInterruptions(JOY_UPDOWN, ON);
}

void initialize_timerb()
{
	halTimer_b_initialize(TIMER_CLKSRC_ACLK, TIMER_MODE_UP);
	halTimer_b_setCCRTimedInterruption(TIMER_B_CCR0, time * timer_multiplier);
	halTimer_b_setInterruptions(ON);
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer (good dog)

    _disable_interrupt();

    initialize_leds();
    initialize_lcd();
    initialize_buttons();
    initialize_timerb();

    _enable_interrupt();

    do
    {
    }
    while ( 1 );
}

#pragma vector = PORT2_VECTOR
__interrupt void on_button_interruption(void)
{
	halJoystick_setInterruptions(JOY_UPDOWN, OFF);

	switch ( P2IFG )
	{
		case JOYSTICK_UP:

			break;
		case JOYSTICK_DOWN:
			if ( timer_multiplier != 1 )
				timer_multiplier = timer_multiplier / 10;
			break;
	}

	P2IFG = 0;
	halJoystick_setInterruptions(JOY_UPDOWN, ON);
}

#pragma vector = TIMERB0_VECTOR
__interrupt void on_timer_b_interruption()
{
	halTimer_b_setCCRInterruption(TIMER_B_CCR0, OFF);

	halLed_sx_toggleLed(LED_SX_ALL);

    halLed_rx_setLed(LED_RX_ALL, OFF);
    halLed_rx_setLed(bitled, ON);

    if ( bitled == LED_R8 )
        bitled = BIT0;
    else
        bitled = bitled << 1;

    halTimer_b_setCCRInterruption(TIMER_B_CCR0, ON);
}
