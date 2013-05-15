/*
 * @file test_accel.c
 */

#include <stdio.h>
#include <hal.h>
#include <kernel.h>
#include "test_accel.h"

extern char __lcd_buffer[17];

int __test_accel_dx = 0;
int __test_accel_dy = 0;
int __test_accel_dz = 0;

int __test_accel_updateVector = FALSE;
int __test_accel_killProgram = FALSE;

static void onProgramStart()
{
    halAccelerometerInit();

    halTimer_b_enableInterruptCCR0();

    halLcdPrintLine("ACCEL. VECTOR", 0, INVERT_TEXT);

    /*   012345
     * 0 ACCEL.VECTOR
     * 1
     * 2   X: 0000
     * 3   Y: 0000
     * 4   Z: 0000
     * */
    halLcdPrintLineCol("X:", 2, 2, OVERWRITE_TEXT);
    halLcdPrintLineCol("Y:", 3, 2, OVERWRITE_TEXT);
    halLcdPrintLineCol("Z:", 4, 2, OVERWRITE_TEXT);

    __test_accel_killProgram = FALSE;
    __test_accel_updateVector = FALSE;

    TB0CCR0 = 32 * 25; // Each 25 milliseconds it will update the window state
}

static void onProgramUpdate()
{
    if ( __test_accel_killProgram )
    {
        kerMenu_exitProgram();
    }
    else if ( __test_accel_updateVector )
    {
        halAccelerometerRead( &__test_accel_dx, &__test_accel_dy, &__test_accel_dz );

        sprintf(__lcd_buffer, "%04d", __test_accel_dx);
        halLcdPrintLineCol(__lcd_buffer, 2, 5, OVERWRITE_TEXT);
        sprintf(__lcd_buffer, "%04d", __test_accel_dy);
        halLcdPrintLineCol(__lcd_buffer, 3, 5, OVERWRITE_TEXT);
        sprintf(__lcd_buffer, "%04d", __test_accel_dz);
        halLcdPrintLineCol(__lcd_buffer, 4, 5, OVERWRITE_TEXT);

        __test_accel_updateVector = FALSE;
    }
}

static void onButtonPressed()
{
    switch ( P2IFG )
    {
        case BUTTON_S2:
            halTimer_b_disableInterruptCCR0();
            __test_accel_killProgram = TRUE;
            break;
    }
}

static void onProgramStop()
{
    halAccelerometerShutDown();
}

static void onTimerB0Interruption()
{
    __test_accel_updateVector = TRUE;
}

void test_accel_bootstrap()
{
    kerMenu_registerProgram("test accel", &onProgramStart,
            &onProgramUpdate, &onProgramStop,
            &onButtonPressed, NULL,
            &onTimerB0Interruption);
}
