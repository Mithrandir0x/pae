/*
 * @file test_motors.c
 */

#include <stdio.h>
#include <hal.h>
#include <kernel.h>
#include "../../lib/motor.h"
#include "test_motors.h"

volatile int __TEST_MOTORS_MOVE_FORWARD = FALSE;
volatile int __TEST_MOTORS_MOVE_BACKWARD = FALSE;
volatile int __TEST_MOTORS_STOP = FALSE;
volatile int __TEST_MOTORS_TURN_LEFT = FALSE;
volatile int __TEST_MOTORS_TURN_RIGHT = FALSE;

volatile int __test_motors_signal_exit = FALSE;

static void test_motors_on_program_start()
{
    halLcdPrintLine("TEST MOTORS", 0, INVERT_TEXT);

    halLed_sx_setLed(LED_RED, ON);
    halLed_sx_setLed(LED_YELLOW, OFF);

    halLed_rx_setLed(LED_RX_ALL, OFF);

    halJoystick_initialize();
    halJoystick_setInterruptions(JOYSTICK_ALL, ON);

    halButtons_initialize();
    halButtons_setInterruptions(BUTTON_ALL, ON);

    halTimer_a1_enableInterruptCCR0();
    halTimer_b_enableInterruptCCR0();

    TB0CCR0 = 32 * 1000; // Interrupt every second

    halBioCom_initialize();

    motor_setSpeed(256);
}

static void test_motors_on_program_update()
{
    if ( __test_motors_signal_exit )
    {
        kerMenu_exitProgram();
    }
    else
    {
        if ( __TEST_MOTORS_MOVE_FORWARD )
        {
            __TEST_MOTORS_MOVE_FORWARD = FALSE;
            motor_advance();
            halLcdPrintLine("  ADVANCING    ", 4, OVERWRITE_TEXT | INVERT_TEXT);
        }

        if ( __TEST_MOTORS_MOVE_BACKWARD )
        {
            __TEST_MOTORS_MOVE_BACKWARD = FALSE;
            motor_retreat();
            halLcdPrintLine("  RETREATING   ", 4, OVERWRITE_TEXT | INVERT_TEXT);
        }

        if ( __TEST_MOTORS_STOP )
        {
            __TEST_MOTORS_STOP = FALSE;
            motor_stop();
            halLcdPrintLine("  STOPPED      ", 4, OVERWRITE_TEXT | INVERT_TEXT);
        }

        if ( __TEST_MOTORS_TURN_LEFT )
        {
            __TEST_MOTORS_TURN_LEFT = FALSE;
            motor_turnLeft();
            halLcdPrintLine("  TURNING LEFT ", 4, OVERWRITE_TEXT | INVERT_TEXT);
        }

        if ( __TEST_MOTORS_TURN_RIGHT )
        {
            __TEST_MOTORS_TURN_RIGHT = FALSE;
            motor_turnRight();
            halLcdPrintLine("  TURNING RIGHT", 4, OVERWRITE_TEXT | INVERT_TEXT);
        }
    }
}

static void test_motors_on_program_stop()
{
    motor_stop();
    motor_setSpeed(0);

    halLed_sx_setLed(LED_SX_ALL, OFF);

    __TEST_MOTORS_MOVE_FORWARD = FALSE;
    __TEST_MOTORS_MOVE_BACKWARD = FALSE;
    __TEST_MOTORS_STOP = FALSE;
    __TEST_MOTORS_TURN_LEFT = FALSE;
    __TEST_MOTORS_TURN_RIGHT = FALSE;
    __test_motors_signal_exit = FALSE;

    TB0CCR0 = 0;
    TA1CCR0 = 0;

    halBioCom_shutdown();
    halLed_sx_shutdown();

    halTimer_a1_disableInterruptCCR0();
    halTimer_b_disableInterruptCCR0();
}

static void test_motors_on_timer_b0_isr()
{
    halLed_sx_toggleLed(LED_SX_ALL);
}

static void test_motors_on_timer_a1_isr()
{
    halBioCom_isr_timer_update();
}

static void test_motors_on_button_pressed()
{
    switch ( P2IFG )
    {
        case JOYSTICK_RIGHT:
            __TEST_MOTORS_TURN_RIGHT = TRUE;
            break;
        case JOYSTICK_LEFT:
            __TEST_MOTORS_TURN_LEFT = TRUE;
            break;
        case JOYSTICK_UP:
            __TEST_MOTORS_MOVE_FORWARD = TRUE;
            break;
        case JOYSTICK_DOWN:
            __TEST_MOTORS_MOVE_BACKWARD = TRUE;
            break;
        case JOYSTICK_CENTER:
            __TEST_MOTORS_STOP = TRUE;
            break;
        case BUTTON_S1:
            __TEST_MOTORS_STOP = TRUE;
            break;
        case BUTTON_S2:
            __test_motors_signal_exit = TRUE;
            break;
    }
}

void test_motors_bootstrap()
{
    kerMenu_registerProgram("test motors", &test_motors_on_program_start,
            &test_motors_on_program_update, &test_motors_on_program_stop,
            &test_motors_on_button_pressed, &test_motors_on_timer_a1_isr,
            &test_motors_on_timer_b0_isr);
}

