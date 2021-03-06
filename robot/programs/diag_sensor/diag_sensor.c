
#include <stdio.h>
#include <hal.h>
#include <kernel.h>

#define DIAG_SENSOR_IR         0
#define DIAG_SENSOR_LUMINOSITY 1

int __diag_sensor_updateData = FALSE;
int __diag_sensor_pollMode = DIAG_SENSOR_IR;

int __diag_sensor_killProgram = FALSE;

extern char __lcd_buffer[17];

static void on_program_start()
{
    halLcdPrintLine("SENSOR DIAG.", 0, INVERT_TEXT);

    halTimer_b_enableInterruptCCR0();
    halTimer_a1_enableInterruptCCR0();

    halBioCom_initialize();

    __diag_sensor_updateData = FALSE;
    __diag_sensor_pollMode = DIAG_SENSOR_IR;
    __diag_sensor_killProgram = FALSE;

    TB0CCR0 = 32 * 25; // Each 25 milliseconds it will update the window state
}

static void on_program_update()
{
    SENSOR_DATA data;

    if ( __diag_sensor_killProgram )
    {
        kerMenu_exitProgram();
    }
    else if ( __diag_sensor_updateData )
    {
        if ( __diag_sensor_pollMode == DIAG_SENSOR_IR )
        {
            halLcdPrintLine("MODE: IR ", 1, OVERWRITE_TEXT);
            data = kerBioAXS1_getIR(100);
        }
        else if ( __diag_sensor_pollMode == DIAG_SENSOR_LUMINOSITY )
        {
            halLcdPrintLine("MODE: LUM", 1, OVERWRITE_TEXT);
            data = kerBioAXS1_getLuminosity(100);
        }

        sprintf(__lcd_buffer, "  LEFT: %03d", data.left);
        halLcdPrintLineCol(__lcd_buffer, 3, 1, OVERWRITE_TEXT);

        sprintf(__lcd_buffer, "CENTER: %03d", data.center);
        halLcdPrintLineCol(__lcd_buffer, 4, 1, OVERWRITE_TEXT);

        sprintf(__lcd_buffer, " RIGHT: %03d", data.right);
        halLcdPrintLineCol(__lcd_buffer, 5, 1, OVERWRITE_TEXT);
    }
}

static void on_program_stop()
{
    TB0CCR0 = 0;
    TA1CCR0 = 0;

    halTimer_b_disableInterruptCCR0();
    halTimer_a1_disableInterruptCCR0();

    halBioCom_shutdown();
}

static void on_timer_b0_isr()
{
    __diag_sensor_updateData = TRUE;
}

static void on_timer_a1_isr()
{
    halBioCom_isr_timer_update();
}

static void on_button_pressed()
{
    switch ( P2IFG )
    {
        case JOYSTICK_CENTER:
            if ( __diag_sensor_pollMode == DIAG_SENSOR_IR )
                __diag_sensor_pollMode = DIAG_SENSOR_LUMINOSITY;
            else
                __diag_sensor_pollMode = DIAG_SENSOR_IR;
            break;
        case BUTTON_S2:
            __diag_sensor_killProgram = TRUE;
            break;
    }
}

void diag_sensor_bootstrap()
{
    kerMenu_registerProgram("diag sensor", &on_program_start,
            &on_program_update, &on_program_stop,
            &on_button_pressed, &on_timer_a1_isr,
            &on_timer_b0_isr);
}
