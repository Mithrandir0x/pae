/*
 * @file test_motors.c
 */

#include <stdio.h>
#include <hal.h>
#include <kernel.h>
#include "../../lib/motor.h"

volatile int __TEST_MOTORS_MOVE_FORWARD = FALSE;
volatile int __TEST_MOTORS_MOVE_BACKWARD = FALSE;
volatile int __TEST_MOTORS_STOP = FALSE;
volatile int __TEST_MOTORS_TURN_LEFT = FALSE;
volatile int __TEST_MOTORS_TURN_RIGHT = FALSE;

static void test_motors_on_program_start()
{
    _disable_interrupt();

    halLed_sx_setLed(LED_RED, ON);
    halLed_sx_setLed(LED_YELLOW, OFF);

    halLed_rx_setLed(LED_RX_ALL, OFF);

    halTimer_b_setCCRTimedInterruption(TIMER_CCR0, 1000);

    halJoystick_initialize();
    halJoystick_setInterruptions(JOYSTICK_ALL, ON);

    halButtons_initialize();
    halButtons_setInterruptions(BUTTON_ALL, ON);

    _enable_interrupt();

    motor_setSpeed(256);

    halBioCom_initialize();
}

static void test_motors_on_program_update()
{
    if ( __TEST_MOTORS_MOVE_FORWARD )
    {
        __TEST_MOTORS_MOVE_FORWARD = FALSE;
        motor_advance();
    }

    if ( __TEST_MOTORS_MOVE_BACKWARD )
    {
        __TEST_MOTORS_MOVE_BACKWARD = FALSE;
        motor_retreat();
    }

    if ( __TEST_MOTORS_STOP )
    {
        __TEST_MOTORS_STOP = FALSE;
        motor_stop();
    }

    if ( __TEST_MOTORS_TURN_LEFT )
    {
        __TEST_MOTORS_TURN_LEFT = FALSE;
        motor_turnLeft();
    }

    if ( __TEST_MOTORS_TURN_RIGHT )
    {
        __TEST_MOTORS_TURN_RIGHT = FALSE;
        motor_turnRight();
    }
}

static void test_motors_on_program_stop()
{
    motor_stop();
    motor_setSpeed(0);

    halBioCom_shutdown();
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
        case BUTTON_S1:
            __TEST_MOTORS_STOP = TRUE;
            break;
        case BUTTON_S2:
            kerMenu_exitProgram();
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

