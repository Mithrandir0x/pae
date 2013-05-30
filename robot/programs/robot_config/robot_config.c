/*
 * @file robot_config.c
 */

#include <stdio.h>
#include <hal.h>
#include <kernel.h>

#include "../robot/robot.h"

#define VARMAX 4

typedef byte (*F_PTR_GET)();
typedef void (*F_PTR_SET)(byte);

typedef struct {
    char tag[5];
    F_PTR_GET getter;
    F_PTR_SET setter;
} GETSET;

extern char __lcd_buffer[17];

static int __robot_config_killProgram = FALSE;
static byte __robot_config_aggregator = 1;
static byte __robot_config_paramSelected = 0;

GETSET param_list[VARMAX];

static GETSET GetSet(char* tag, F_PTR_GET getter, F_PTR_SET setter)
{
    GETSET getset;

    sprintf(getset.tag, "%s", tag);
    getset.getter = getter;
    getset.setter = setter;

    return getset;
}

static void onProgramStart()
{
    byte i;

    halLcdPrintLine("ROBOT CONFIG.", 0, INVERT_TEXT);

    for ( i = 0 ; i < VARMAX ; i++ )
    {
        sprintf(__lcd_buffer, "%s: %03d", param_list[i].tag, param_list[i].getter());
        halLcdPrintLineCol(__lcd_buffer, i + 2, 2, OVERWRITE_TEXT);
    }

    __robot_config_killProgram = FALSE;
    __robot_config_aggregator = 1;
    __robot_config_paramSelected = 0;
}

static void clearTickles()
{
    byte i;

    for ( i = 0 ; i < VARMAX ; i++ )
    {
        halLcdPrintLineCol(" ", i + 2, 1, OVERWRITE_TEXT);
    }
}

static void renderTickle()
{
    clearTickles();
    halLcdPrintLineCol("x", __robot_config_paramSelected + 2, 1, INVERT_TEXT);
}

static void onProgramUpdate()
{
    byte i;

    if ( __robot_config_killProgram )
    {
        kerMenu_exitProgram();
    }
    else
    {
        renderTickle();

        for ( i = 0 ; i < VARMAX ; i++ )
        {
            sprintf(__lcd_buffer, "%s: %03d", param_list[i].tag, param_list[i].getter());
            halLcdPrintLineCol(__lcd_buffer, i + 2, 2, OVERWRITE_TEXT);
        }
    }
}

static void onProgramStop()
{
}

static void onButtonPressed()
{
    byte v;

    switch ( P2IFG )
    {
        case JOYSTICK_UP:
            if ( __robot_config_paramSelected > 0 )
                __robot_config_paramSelected--;
            break;
        case JOYSTICK_DOWN:
            if ( __robot_config_paramSelected < VARMAX - 1 )
                __robot_config_paramSelected++;
            break;
        case JOYSTICK_LEFT:
            v = param_list[__robot_config_paramSelected].getter();
            param_list[__robot_config_paramSelected].setter(v - __robot_config_aggregator);
            break;
        case JOYSTICK_RIGHT:
            v = param_list[__robot_config_paramSelected].getter();
            param_list[__robot_config_paramSelected].setter(v + __robot_config_aggregator);
            break;
        case BUTTON_S1:
            if ( __robot_config_aggregator == 1 ) // FUGLY AS HELL...
                __robot_config_aggregator = 10;
            else if ( __robot_config_aggregator == 10 )
                __robot_config_aggregator = 50;
            else
                __robot_config_aggregator = 1;
            break;
        case BUTTON_S2:
            __robot_config_killProgram = TRUE;
            break;
    }
}

void robot_config_bootstrap()
{
    param_list[0] = GetSet("INTR", &robot_getUpdateInterval, &robot_setUpdateInterval);
    param_list[1] = GetSet("TH L", &robot_getThresholdLeft, &robot_setThresholdLeft);
    param_list[2] = GetSet("TH R", &robot_getThresholdRight, &robot_setThresholdRight);
    param_list[3] = GetSet("THLA", &robot_getThresholdLeftAlt, &robot_setThresholdLeftAlt);
    //param_list[4] = GetSet(" CPM ", &robot_getConvexPanicMax, &robot_setConvexPanicMax);

    kerMenu_registerProgram("robot conf", &onProgramStart,
                    &onProgramUpdate, &onProgramStop,
                    &onButtonPressed, NULL,
                    NULL);
}
