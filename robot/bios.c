/*
 * @file bios.c
 */

#include <hal.h>
#include <kernel.h>

#include "programs/diag_sensor/diag_sensor.h"
#include "programs/test_motors/test_motors.h"

// Allow other programs to reuse LCD write buffer
extern char __lcd_buffer[17];

void main()
{
    byte lcd_contrast  = 0x64;
    byte lcd_backlight = 30;
    F_PTR callback;

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer (good dog)

    _disable_interrupt();

    halUCS_set16MFrequency();

    halLed_sx_initialize();
    halLed_sx_setLed(LED_SX_ALL, OFF);

    halLed_rx_initialize();
    halLed_rx_setLed(LED_RX_ALL, OFF);

    halJoystick_initialize();
    halJoystick_setInterruptions(JOYSTICK_ALL, ON);

    halButtons_initialize();
    halButtons_setInterruptions(BUTTON_ALL, ON);

    halTimer_b_initialize(TIMER_CLKSRC_ACLK, TIMER_MODE_UP);
    halTimer_b_enableInterruptCCR0();

    halTimer_a1_initialize(TIMER_CLKSRC_SMCLK, TIMER_MODE_UP);
    halTimer_a1_enableInterruptCCR0();

    halLcdInit();
    halLcdBackLightInit();
    halLcdSetBackLight(lcd_backlight);
    halLcdSetContrast(lcd_contrast);
    halLcdClearScreen();

    _enable_interrupt();

    kerMenu_bootstrap();

    diag_sensor_bootstrap();
    //test_motors_bootstrap();

    while (1)
    {
        callback = kerMenu_getOnProgramUpdateCallback();

        if ( callback != NULL )
        {
            callback();
        }
    }
}

#pragma vector = PORT2_VECTOR
__interrupt void __PORT2_ISR()
{
    F_PTR callback;

    halButtons_setInterruptions(BUTTON_ALL, OFF);
    halJoystick_setInterruptions(JOYSTICK_ALL, OFF);

    callback = kerMenu_getOnButtonPressedCallback();

    if ( callback != NULL )
    {
        callback();
    }

    P2IFG = 0;

    halButtons_setInterruptions(BUTTON_ALL, ON);
    halJoystick_setInterruptions(JOYSTICK_ALL, ON);
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void __TIMER_A1_ISR()
{
    F_PTR callback = kerMenu_getOnTimerA1InterruptCallback();

    if ( callback != NULL )
    {
        callback();
    }
}

#pragma vector = TIMERB0_VECTOR
__interrupt void __TIMER_B0_ISR()
{
    F_PTR callback = kerMenu_getOnTimerB0InterruptCallback();

    if ( callback != NULL )
    {
        callback();
    }
}
