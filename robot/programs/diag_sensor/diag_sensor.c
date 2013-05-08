
#include <stdio.h>
#include <hal.h>
#include <kernel.h>

#define DIAG_SENSOR_IR         0
#define DIAG_SENSOR_LUMINOSITY 1

int __diag_sensor_updateData = FALSE;
int __diag_sensor_pollMode = DIAG_SENSOR_IR;

byte __diag_sensor_stoopidCounter = 0;

extern char __lcd_buffer[17];

void diag_sensor_on_program_start()
{
    halLcdPrintLine("SENSOR DIAG.", 0, INVERT_TEXT);

    halTimer_a1_enableInterruptCCR0();

    halBioCom_initialize();

    __diag_sensor_updateData = FALSE;
    __diag_sensor_pollMode = DIAG_SENSOR_IR;

    TB0CCR0 = 32 * 25; // Each 25 milliseconds it will update the window state
}

void diag_sensor_on_program_update()
{
    SENSOR_DATA data;

    if ( __diag_sensor_updateData )
    {
        if ( __diag_sensor_pollMode == DIAG_SENSOR_IR )
            data = kerBioAXS1_getIR(100);
        else if ( __diag_sensor_pollMode == DIAG_SENSOR_LUMINOSITY )
            data = kerBioAXS1_getLuminosity(100);

        sprintf(__lcd_buffer, "  LEFT: %03d", data.left);
        halLcdPrintLineCol(__lcd_buffer, 3, 1, OVERWRITE_TEXT);

        sprintf(__lcd_buffer, "CENTER: %03d", data.center);
        halLcdPrintLineCol(__lcd_buffer, 4, 1, OVERWRITE_TEXT);

        sprintf(__lcd_buffer, " RIGHT: %03d", data.right);
        halLcdPrintLineCol(__lcd_buffer, 5, 1, OVERWRITE_TEXT);
    }
}

void diag_sensor_on_program_stop()
{
    halBioCom_shutdown();
}

void diag_sensor_on_timer_b0_isr()
{
    __diag_sensor_updateData = TRUE;
}

void diag_sensor_on_timer_a1_isr()
{
    halBioCom_isr_timer_update();
}

void diag_sensor_on_button_pressed()
{
    switch ( P2IE )
    {
        case BUTTON_S1:
            __diag_sensor_pollMode = DIAG_SENSOR_IR;
            break;
        case BUTTON_S2:
            __diag_sensor_pollMode = DIAG_SENSOR_LUMINOSITY;
            break;
    }

    P2IE = 0;
}

void diag_sensor_bootstrap()
{
    kerMenu_registerProgram("diag sensor", &diag_sensor_on_program_start,
            &diag_sensor_on_program_update, &diag_sensor_on_program_stop,
            &diag_sensor_on_button_pressed, &diag_sensor_on_timer_a1_isr,
            &diag_sensor_on_timer_b0_isr);
}
