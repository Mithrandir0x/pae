/*
 * @file kernel_menu.c
 */

#include <stdio.h>

#include <hal_buttons.h>
#include <hal_common.h>
#include <hal_lcd.h>
#include "kernel_menu.h"

#define __KERNEL_MENU_MAX_PROGRAMS 16
#define __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE 4
#define __KERNEL_MENU_MAX_PAGES ( __KERNEL_MENU_MAX_PROGRAMS / __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE )

#define KERNEL_MENU_PROGRAM_ID ( __KERNEL_MENU_MAX_PROGRAMS )

#define __KERNEL_MENU_EXECUTING_PROGRAM ( __kernel_menu_programs[__kernel_menu_executingProgram] )

typedef struct {
    char tag[13];
    F_PTR onButtonPressedCallback;
    F_PTR onTimerA1InterruptCallback;
    F_PTR onTimerB0InterruptCallback;
    F_PTR onProgramInitializeCallback;
    F_PTR onProgramUpdateCallback;
    F_PTR onProgramStopCallback;
} PR_CTX; // Program Context

unsigned int __kernel_menu_storedPrograms = 0;
unsigned int __kernel_menu_selectedProgram = 0;

int __kernel_menu_executingProgram = KERNEL_MENU_PROGRAM_ID;

int __kernel_menu_updateTick = FALSE;
int __kernel_menu_updateMenu = FALSE;

PR_CTX __kernel_menu_programs[__KERNEL_MENU_MAX_PROGRAMS + 1];
char __lcd_buffer[17];

void renderTickle()
{
    halLcdPrintLineCol(" ", ( __kernel_menu_selectedProgram % __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE ) - 1, 1, OVERWRITE_TEXT);
    halLcdPrintLineCol("x", __kernel_menu_selectedProgram % __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE, 1, INVERT_TEXT);
}

void renderMenu()
{
    int i = 0;
    int page = __kernel_menu_selectedProgram % __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE;
    int lastProgram = __kernel_menu_storedPrograms + 4;

    if ( lastProgram >= __KERNEL_MENU_MAX_PROGRAMS )
        lastProgram = __KERNEL_MENU_MAX_PROGRAMS;

    halLcdClearScreen();
    halLcdPrintLineCol("BOT MK1", 0, 1, OVERWRITE_TEXT);

    {   // Write program page number XX/XX
        sprintf(__lcd_buffer, "%02d:%02d", page + 1, __KERNEL_MENU_MAX_PAGES);
        halLcdPrintLineCol(__lcd_buffer, 0, 11, OVERWRITE_TEXT);
    }

    {   // Write programs available at current page
        for ( i = 0 ; i < lastProgram ; i++ )
        {
            halLcdPrintLineCol(__kernel_menu_programs[page + i].tag, i + 2, 3, OVERWRITE_TEXT);
        }
    }

    // Write currently selected program
    halLcdPrintLineCol("x", __kernel_menu_selectedProgram % __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE, 1, INVERT_TEXT);
}

void onMenuShutdown()
{
    halLcdClearScreen();
}

void onButtonPressed()
{
    F_PTR callback;
    int page = __kernel_menu_selectedProgram % __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE;

    switch ( P2IFG )
    {
        case JOYSTICK_RIGHT:
            if ( page < __KERNEL_MENU_MAX_PAGES )
            {
                __kernel_menu_selectedProgram = ( page - 1 ) * __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE;
                __kernel_menu_updateMenu = TRUE;
            }
            break;
        case JOYSTICK_LEFT:
            if ( page > 0 )
            {
                __kernel_menu_selectedProgram = ( page + 1 ) * __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE;
                __kernel_menu_updateMenu = TRUE;
            }
            break;
        case JOYSTICK_UP:
            if ( __kernel_menu_selectedProgram > 0 )
            {
                __kernel_menu_selectedProgram--;
                __kernel_menu_updateTick = TRUE;
            }

            if ( page != __kernel_menu_selectedProgram % __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE )
                __kernel_menu_updateMenu = TRUE;
            break;
        case JOYSTICK_DOWN:
            if ( __kernel_menu_selectedProgram < __kernel_menu_storedPrograms )
            {
                __kernel_menu_selectedProgram++;
                __kernel_menu_updateTick = TRUE;
            }

            if ( page != __kernel_menu_selectedProgram % __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE )
                __kernel_menu_updateMenu = TRUE;
            break;
        case JOYSTICK_CENTER:
            if ( __kernel_menu_selectedProgram != __kernel_menu_executingProgram )
            {
                onMenuShutdown();

                callback = __kernel_menu_programs[__kernel_menu_selectedProgram].onProgramInitializeCallback;
                if ( callback != NULL ) callback();

                __kernel_menu_executingProgram = __kernel_menu_selectedProgram;
            }
            break;
        case BUTTON_S1:
            break;
        case BUTTON_S2:
            break;
    }
}

void onMenuUpdate()
{
    if ( __kernel_menu_updateMenu )
    {
        __kernel_menu_updateMenu = FALSE;
        renderMenu();
    }
    else if ( __kernel_menu_updateTick )
    {
        __kernel_menu_updateTick = FALSE;
        renderTickle();
    }
}

/**
 * This function initializes program data.
 */
void kerMenu_bootstrap()
{
    int i;
    PR_CTX program = __kernel_menu_programs[KERNEL_MENU_PROGRAM_ID];

    halLcdSetBackLight(0x64);
    halLcdSetContrast(0x1E);

    // Initialize every program slot
    for ( i = 0 ; i < __KERNEL_MENU_MAX_PROGRAMS + 1 ; i++ )
    {
        program = __kernel_menu_programs[i];
        sprintf(program.tag, "%13c", "");
        program.onButtonPressedCallback = NULL;
        program.onTimerA1InterruptCallback = NULL;
        program.onTimerB0InterruptCallback = NULL;
        program.onProgramInitializeCallback = NULL;
        program.onProgramUpdateCallback = NULL;
        program.onProgramStopCallback = NULL;
    }

    program = __kernel_menu_programs[KERNEL_MENU_PROGRAM_ID];

    sprintf(program.tag, "%13c", "MENU");
    program.onButtonPressedCallback = &onButtonPressed;
    program.onTimerA1InterruptCallback = NULL;
    program.onTimerB0InterruptCallback = NULL;
    program.onProgramInitializeCallback = NULL;
    program.onProgramUpdateCallback = &onMenuUpdate;
    program.onProgramStopCallback = &onMenuShutdown;

    __kernel_menu_updateMenu = TRUE;
}

/**
 * This function allows to register a program execution context.
 *
 * @param tag                           The name of the program when listed in the menu.
 * @param onProgramInitializeCallback   A pointer to the function to be called when the program is to be initialized.
 * @param onProgramUpdateCallback       A pointer to the function to be called when the program state is to be updated.
 * @param onProgramStopCallback         A pointer to the function to be called when the program is to be shutdown.
 * @param onButtonPressedCallback       A pointer to the function to be called when a button interruption is risen.
 * @param onTimerA1InterruptionCallback A pointer to the function to be called when the Timer A1 rises an interruption.
 * @param onTimerB0InterruptionCallback A pointer to the function to be called when the Timer B0 rises an interruption.
 */
void kerMenu_registerProgram(char* tag,
        F_PTR onProgramInitializeCallback,
        F_PTR onProgramUpdateCallback,
        F_PTR onProgramStopCallback,
        F_PTR onButtonPressedCallback,
        F_PTR onTimerA1InterruptCallback,
        F_PTR onTimerB0InterruptCallback) {
    PR_CTX newProgram;

    if ( __kernel_menu_storedPrograms < __KERNEL_MENU_MAX_PROGRAMS )
    {
        sprintf(newProgram.tag, "%13c", tag);
        newProgram.onButtonPressedCallback = onButtonPressedCallback;
        newProgram.onTimerA1InterruptCallback = onTimerA1InterruptCallback;
        newProgram.onTimerB0InterruptCallback = onTimerB0InterruptCallback;
        newProgram.onProgramInitializeCallback = onProgramInitializeCallback;
        newProgram.onProgramUpdateCallback = onProgramUpdateCallback;
        newProgram.onProgramStopCallback = onProgramStopCallback;

        __kernel_menu_programs[__kernel_menu_storedPrograms] = newProgram;
        __kernel_menu_storedPrograms++;
    }
}

void kerMenu_exitProgram()
{
    F_PTR callback;

    callback = __KERNEL_MENU_EXECUTING_PROGRAM.onProgramStopCallback;
    if ( callback != NULL ) callback();

    // We change the executing program index at the end of initialization call
    // because we don't want any interruption to be executed for the new
    // program without proper initialization. Each program has its own context
    // and should not be thinking about any other program.
    __kernel_menu_executingProgram = KERNEL_MENU_PROGRAM_ID;
    __kernel_menu_updateMenu = TRUE;
}

F_PTR kerMenu_getOnButtonPressedCallback()
{
    return __kernel_menu_programs[__kernel_menu_executingProgram].onButtonPressedCallback;
}

F_PTR kerMenu_getOnTimerA1InterruptCallback()
{
    return __kernel_menu_programs[__kernel_menu_executingProgram].onTimerA1InterruptCallback;
}

F_PTR kerMenu_getOnTimerB0InterruptCallback()
{
    return __kernel_menu_programs[__kernel_menu_executingProgram].onTimerB0InterruptCallback;
}

F_PTR kerMenu_getOnProgramInitializeCallback()
{
    return __kernel_menu_programs[__kernel_menu_executingProgram].onProgramInitializeCallback;
}

F_PTR kerMenu_getOnProgramUpdateCallback()
{
    return __kernel_menu_programs[__kernel_menu_executingProgram].onProgramUpdateCallback;
}

F_PTR kerMenu_getOnProgramStopCallback()
{
    return __kernel_menu_programs[__kernel_menu_executingProgram].onProgramStopCallback;
}
