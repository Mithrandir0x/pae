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
    char tag[12];
    F_PTR onButtonPressedCallback;
    F_PTR onTimerA1InterruptCallback;
    F_PTR onTimerB0InterruptCallback;
    F_PTR onProgramInitializeCallback;
    F_PTR onProgramUpdateCallback;
    F_PTR onProgramStopCallback;
} PR_CTX; // Program Context

byte __kernel_menu_totalPages = 0;
byte __kernel_menu_storedPrograms = 0;
byte __kernel_menu_selectedProgram = 0;

int __kernel_menu_executingProgram = KERNEL_MENU_PROGRAM_ID;

int __kernel_menu_updateTick = FALSE;
int __kernel_menu_updateMenu = FALSE;
int __kernel_menu_initializeProgram = FALSE;

PR_CTX __kernel_menu_programs[__KERNEL_MENU_MAX_PROGRAMS + 1];

static void clearTickles()
{
    byte i;

    for ( i = 0 ; i < __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE ; i++ )
    {
        halLcdPrintLineCol(" ", i + 2, 1, OVERWRITE_TEXT);
    }
}

static void renderTickle()
{
    clearTickles();
    halLcdPrintLineCol("x", ( __kernel_menu_selectedProgram % __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE ) + 2, 1, INVERT_TEXT);
}

static void renderMenu()
{
    byte i;
    byte page = __kernel_menu_selectedProgram / __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE;
    char __lcd_buffer[17];

    halLcdClearScreen();
    halLcdPrintLineCol("BOT MK1", 0, 1, OVERWRITE_TEXT);

    {   // Write program page number X/X
        sprintf(__lcd_buffer, "%d:%d", page + 1, __kernel_menu_totalPages + 1);
        halLcdPrintLineCol(__lcd_buffer, 0, 13, OVERWRITE_TEXT);
    }

    {   // Write programs available at current page
        for ( i = 0 ; i < __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE && i < __kernel_menu_storedPrograms ; i++ )
        {
            halLcdPrintLineCol(__kernel_menu_programs[( page * __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE ) + i].tag, i + 2, 3, OVERWRITE_TEXT);
        }
    }

    // Write currently selected program
    renderTickle();
}

static void onMenuShutdown()
{
    halLcdClearScreen();
}

static void onButtonPressed()
{
    byte page;

    page = __kernel_menu_selectedProgram / __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE;

    switch ( P2IFG )
    {
        case JOYSTICK_RIGHT:
            if ( page < __KERNEL_MENU_MAX_PAGES && page < __kernel_menu_totalPages )
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

            if ( page != ( __kernel_menu_selectedProgram / __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE ) )
                __kernel_menu_updateMenu = TRUE;
            break;
        case JOYSTICK_DOWN:
            if ( __kernel_menu_selectedProgram < __kernel_menu_storedPrograms )
            {
                __kernel_menu_selectedProgram++;
                __kernel_menu_updateTick = TRUE;
            }

            if ( page != ( __kernel_menu_selectedProgram / __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE ) )
                __kernel_menu_updateMenu = TRUE;
            break;
        case JOYSTICK_CENTER:
            if ( __kernel_menu_selectedProgram != __kernel_menu_executingProgram )
            {
                onMenuShutdown();

                __kernel_menu_executingProgram = __kernel_menu_selectedProgram;
                __kernel_menu_initializeProgram = TRUE;
            }
            break;
        case BUTTON_S1:
            break;
        case BUTTON_S2:
            break;
    }
}

static void onMenuUpdate()
{
    F_PTR callback;

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

    if ( __kernel_menu_initializeProgram )
    {
        callback = __kernel_menu_programs[__kernel_menu_selectedProgram].onProgramInitializeCallback;
        if ( callback != NULL ) callback();

        __kernel_menu_initializeProgram = FALSE;
    }
}

/**
 * This function initializes program data.
 */
void kerMenu_bootstrap()
{
    byte i;

    // Initialize every program slot
    for ( i = 0 ; i < __KERNEL_MENU_MAX_PROGRAMS + 1 ; i++ )
    {
        __kernel_menu_programs[i].onButtonPressedCallback = 0;
        sprintf(__kernel_menu_programs[i].tag, "%s", "            ");
        __kernel_menu_programs[i].onTimerA1InterruptCallback = NULL;
        __kernel_menu_programs[i].onTimerB0InterruptCallback = NULL;
        __kernel_menu_programs[i].onProgramInitializeCallback = NULL;
        __kernel_menu_programs[i].onProgramUpdateCallback = NULL;
        __kernel_menu_programs[i].onProgramStopCallback = NULL;
    }

    sprintf(__kernel_menu_programs[KERNEL_MENU_PROGRAM_ID].tag, "%s", "MENU        ");
    __kernel_menu_programs[KERNEL_MENU_PROGRAM_ID].onButtonPressedCallback = &onButtonPressed;
    __kernel_menu_programs[KERNEL_MENU_PROGRAM_ID].onTimerA1InterruptCallback = NULL;
    __kernel_menu_programs[KERNEL_MENU_PROGRAM_ID].onTimerB0InterruptCallback = NULL;
    __kernel_menu_programs[KERNEL_MENU_PROGRAM_ID].onProgramInitializeCallback = NULL;
    __kernel_menu_programs[KERNEL_MENU_PROGRAM_ID].onProgramUpdateCallback = &onMenuUpdate;
    __kernel_menu_programs[KERNEL_MENU_PROGRAM_ID].onProgramStopCallback = &onMenuShutdown;

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

    if ( __kernel_menu_storedPrograms < __KERNEL_MENU_MAX_PROGRAMS )
    {
        sprintf(__kernel_menu_programs[__kernel_menu_storedPrograms].tag, "%s", tag);
        __kernel_menu_programs[__kernel_menu_storedPrograms].onButtonPressedCallback = onButtonPressedCallback;
        __kernel_menu_programs[__kernel_menu_storedPrograms].onTimerA1InterruptCallback = onTimerA1InterruptCallback;
        __kernel_menu_programs[__kernel_menu_storedPrograms].onTimerB0InterruptCallback = onTimerB0InterruptCallback;
        __kernel_menu_programs[__kernel_menu_storedPrograms].onProgramInitializeCallback = onProgramInitializeCallback;
        __kernel_menu_programs[__kernel_menu_storedPrograms].onProgramUpdateCallback = onProgramUpdateCallback;
        __kernel_menu_programs[__kernel_menu_storedPrograms].onProgramStopCallback = onProgramStopCallback;

        __kernel_menu_storedPrograms++;
        __kernel_menu_totalPages = __kernel_menu_storedPrograms / __KERNEL_MENU_MAX_PROGRAMS_PER_PAGE;
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
