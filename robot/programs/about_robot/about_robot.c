/*
 * @file about_robot.c
 */

#include <hal.h>
#include <kernel.h>
#include "about_robot.h"

static void onProgramStart()
{
}

void aboutRobot_bootstrap()
{
    kerMenu_registerProgram("about robot", &onProgramStart, NULL, NULL, NULL, NULL, NULL);
}
