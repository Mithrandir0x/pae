/*
 * @file kernel_menu.h
 */

#ifndef _KERNEL_MENU_H_
#define _KERNEL_MENU_H_

typedef void (*F_PTR)();

void kerMenu_bootstrap();

void kerMenu_registerProgram(char* tag,
        F_PTR onProgramInitializeCallback,
        F_PTR onProgramUpdateCallback,
        F_PTR onProgramStopCallback,
        F_PTR onButtonPressedCallback,
        F_PTR onTimerA1InterruptCallback,
        F_PTR onTimerB0InterruptCallback);

F_PTR kerMenu_getOnButtonPressedCallback();
F_PTR kerMenu_getOnTimerA1InterruptCallback();
F_PTR kerMenu_getOnTimerB0InterruptCallback();
//F_PTR kerMenu_getOnProgramInitializeCallback();
F_PTR kerMenu_getOnProgramUpdateCallback();
//F_PTR kerMenu_getOnProgramStopCallback();

void kerMenu_exitProgram();

#endif
