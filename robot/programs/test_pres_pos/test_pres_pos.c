/*
 * @file test_pres_pos.c
 */

#include <stdio.h>
#include <hal.h>
#include <kernel.h>
#include "../../lib/motor.h"
#include "test_pres_pos.h"

volatile int __TEST_PRES_POS_updatePresPos = FALSE;

volatile int __TEST_PRES_POS_MOVE_FORWARD = FALSE;
volatile int __TEST_PRES_POS_MOVE_BACKWARD = FALSE;
volatile int __TEST_PRES_POS_STOP = FALSE;

extern char __lcd_buffer[17];

int __test_pres_pos_killProgram = FALSE;

static void onProgramStart()
{
    halTimer_b_enableInterruptCCR0();
    halTimer_a1_enableInterruptCCR0();
    halBioCom_initialize();

    halLcdPrintLine("CURRENT POS.", 0, INVERT_TEXT);

    motor_setSpeed(32);

    __test_pres_pos_killProgram = FALSE;

    TB0CCR0 = 32 * 25; // Each 25 milliseconds it will update the window state
}

static void onProgramUpdate()
{
    int data;

    if ( __test_pres_pos_killProgram )
    {
        kerMenu_exitProgram();
    }
    else if ( __TEST_PRES_POS_MOVE_FORWARD )
    {
        __TEST_PRES_POS_MOVE_FORWARD = FALSE;
        motor_advance();
    }
    else if ( __TEST_PRES_POS_MOVE_BACKWARD )
    {
        __TEST_PRES_POS_MOVE_BACKWARD = FALSE;
        motor_retreat();
    }
    else if ( __TEST_PRES_POS_STOP )
    {
        __TEST_PRES_POS_STOP = FALSE;
        motor_stop();
    }
    else if ( __TEST_PRES_POS_updatePresPos )
    {
        __TEST_PRES_POS_updatePresPos = FALSE;

        data = kerBioAX12_getPresentSpeed(3);

        sprintf(__lcd_buffer, "  POS: %04d", data);
        halLcdPrintLineCol(__lcd_buffer, 3, 1, OVERWRITE_TEXT);
    }
}

static void onButtonPressed()
{
    switch ( P2IFG )
    {
        case JOYSTICK_UP:
            __TEST_PRES_POS_MOVE_FORWARD = TRUE;
            break;
        case JOYSTICK_DOWN:
            __TEST_PRES_POS_MOVE_BACKWARD = TRUE;
            break;
        case JOYSTICK_CENTER:
            __TEST_PRES_POS_STOP = TRUE;
            break;
        case BUTTON_S2:
            halTimer_a1_disableInterruptCCR0();
            halTimer_b_disableInterruptCCR0();
            __test_pres_pos_killProgram = TRUE;
            break;
    }

    P2IFG = 0;
}

static void onProgramStop()
{
    __TEST_PRES_POS_MOVE_FORWARD = FALSE;
    __TEST_PRES_POS_MOVE_BACKWARD = FALSE;
    __TEST_PRES_POS_STOP = FALSE;

    halBioCom_shutdown();
}

static void onTimerB0Interruption()
{
    __TEST_PRES_POS_updatePresPos = TRUE;
}

static void onTimerA1Interruption()
{
    halBioCom_isr_timer_update();
}


void test_pres_pos_bootstrap()
{
    kerMenu_registerProgram("test pos", &onProgramStart,
            &onProgramUpdate, &onProgramStop,
            &onButtonPressed, &onTimerA1Interruption,
            &onTimerB0Interruption);
}
