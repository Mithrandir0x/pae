/*
 * @file robot.c
 */

#include <stdio.h>
#include <hal.h>
#include <kernel.h>

#include "robot.h"
#include "../../lib/motor.h"

typedef enum { IDLE, ALIGN, MOVE } ROBOT_STATE;

static ROBOT_STATE robotState;

static int killProgram = FALSE;
static int updatedSensorData = FALSE;
static int stopRobot = FALSE;
static int assumingDirectControl = FALSE;

static int __TEST_MOTORS_MOVE_FORWARD = FALSE;
static int __TEST_MOTORS_MOVE_BACKWARD = FALSE;
static int __TEST_MOTORS_STOP = FALSE;
static int __TEST_MOTORS_TURN_LEFT = FALSE;
static int __TEST_MOTORS_TURN_RIGHT = FALSE;

static byte INTERVAL = 50;   // Number of milliseconds to wait before updating again the robot state
static byte THRESHOLD_LEFT = 35;
static byte THRESHOLD_RIGHT = 245;
static byte THRESHOLD_LEFT_ALT = 140;
static byte CONVEX_PANIC_MAX = 20;

static int SPEED = 350;

static byte convexPanicCnt = 0;

extern char __lcd_buffer[17];

static void onProgramStart()
{
    //halLcdShutDownBackLight();
    //halLcdShutDown();
    halLcdPrintLine("RO BOT", 0, INVERT_TEXT);

    halTimer_b_enableInterruptCCR0();
    halTimer_a1_enableInterruptCCR0();

    halBioCom_initialize();

    updatedSensorData = FALSE;
    killProgram = FALSE;
    stopRobot = FALSE;
    assumingDirectControl = FALSE;

    convexPanicCnt = 0;

    motor_setSpeed(SPEED);

    convexPanicCnt = CONVEX_PANIC_MAX;
    robotState = IDLE;

    TB0CCR0 = 32 * INTERVAL;
}

static void onProgramUpdate()
{
    F_PTR action;
    SENSOR_DATA data;

    if ( killProgram )
    {
        kerMenu_exitProgram();
    }
    else if ( stopRobot )
    {
        motor_stop();

        stopRobot = FALSE;
    }
    else if ( assumingDirectControl )
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

        assumingDirectControl = FALSE;
    }
    else if ( updatedSensorData )
    {
        data = kerBioAXS1_getIR(100);

        sprintf(__lcd_buffer, "  LEFT: %03d", data.left);
        halLcdPrintLineCol(__lcd_buffer, 3, 1, OVERWRITE_TEXT);

        sprintf(__lcd_buffer, "CENTER: %03d", data.center);
        halLcdPrintLineCol(__lcd_buffer, 4, 1, OVERWRITE_TEXT);

        sprintf(__lcd_buffer, " RIGHT: %03d", data.right);
        halLcdPrintLineCol(__lcd_buffer, 5, 1, OVERWRITE_TEXT);

        if ( robotState == ALIGN )
        {
            // Alignment logic
        }

        if ( robotState == MOVE )
        {
            // I'm silly, so I just want to go ahead
            action = &motor_advance;

            // Woah! An obstacle! I would love to have CUT!
            if ( data.center == 255 )
            {
                // Hey, there's something left to me
                if ( data.left != 0 )
                {
                    action = &motor_moveRight;
                }
                // Nope, there's something right to me
                /*else if ( data.right != 0 )
                {
                    action = &motor_turnLeft;
                }*/
                // Oh shit, IT'S A TRAP!!!
                else if ( data.left != 0 && data.right != 0 )
                {
                    action = &motor_stop;
                    robotState = IDLE;
                }
            }

            // Am I too far from the wall?
            if ( data.left < THRESHOLD_LEFT ) // THRESHOLD_LEFT
            {
                action = &motor_turnLeft;
            }

            // Am I too close to the wall?
            if ( data.left > THRESHOLD_RIGHT ) // THRESHOLD_RIGHT
            {
                action = &motor_turnRight;
            }

            // So, still on track?
            if ( data.left >= THRESHOLD_LEFT_ALT && data.left <= THRESHOLD_RIGHT )
            {
                convexPanicCnt = 0;
            }

            // Hey, where's the wall?
            if ( data.left == 0 )
            {
                convexPanicCnt++;
                action = &motor_advance;
                // I have a hell of a bum, with my current speed,
                // have I advanced enough to turn left graciously
                // without my bum hitting the box?
                if ( convexPanicCnt >= 2 )
                {
                    action = &motor_turnLeft;
                }
            }

            action();
        }

        updatedSensorData = FALSE;
    }
}

static void onProgramStop()
{
    //byte lcd_contrast  = 0x64;
    //byte lcd_backlight = 30;

    motor_stop();
    motor_setSpeed(0);

    TB0CCR0 = 0;
    TA1CCR0 = 0;

    halTimer_b_disableInterruptCCR0();
    halTimer_a1_disableInterruptCCR0();

    halBioCom_shutdown();

    //halLcdInit();
    //halLcdBackLightInit();
    //halLcdSetBackLight(lcd_backlight);
    //halLcdSetContrast(lcd_contrast);
}

static void onButtonPressed()
{
    switch ( P2IFG )
    {
        case JOYSTICK_RIGHT:
            __TEST_MOTORS_TURN_RIGHT = TRUE;
            assumingDirectControl = TRUE;
            break;
        case JOYSTICK_LEFT:
            __TEST_MOTORS_TURN_LEFT = TRUE;
            assumingDirectControl = TRUE;
            break;
        case JOYSTICK_UP:
            __TEST_MOTORS_MOVE_FORWARD = TRUE;
            assumingDirectControl = TRUE;
            break;
        case JOYSTICK_DOWN:
            __TEST_MOTORS_MOVE_BACKWARD = TRUE;
            assumingDirectControl = TRUE;
            break;
        case JOYSTICK_CENTER:
            __TEST_MOTORS_STOP = TRUE;
            assumingDirectControl = TRUE;
            break;
        case BUTTON_S1:
            if ( robotState == IDLE )
            {
                robotState = MOVE;
                convexPanicCnt = 0;
            }
            else
            {
                robotState = IDLE;
                stopRobot = TRUE;
            }
            break;
        case BUTTON_S2:
            killProgram = TRUE;
            break;
    }
}

/**
 * Interesting bug
 */
static void onTimerA1Interruption()
{
    halBioCom_isr_timer_update();
}

static void onTimerB0Interruption()
{
    updatedSensorData = TRUE;
}

void robot_bootstrap()
{
    kerMenu_registerProgram("robot", &onProgramStart,
                &onProgramUpdate, &onProgramStop,
                &onButtonPressed, &onTimerA1Interruption,
                &onTimerB0Interruption);
}

byte robot_getUpdateInterval() { return INTERVAL; }
void robot_setUpdateInterval(byte t) { INTERVAL = t; }

byte robot_getThresholdLeft() { return THRESHOLD_LEFT; }
void robot_setThresholdLeft(byte a) { THRESHOLD_LEFT = a; }

byte robot_getThresholdRight() { return THRESHOLD_RIGHT; }
void robot_setThresholdRight(byte b) { THRESHOLD_RIGHT = b; }

byte robot_getThresholdLeftAlt() { return THRESHOLD_LEFT_ALT; }
void robot_setThresholdLeftAlt(byte c) { THRESHOLD_LEFT_ALT = c; }

byte robot_getConvexPanicMax() { return CONVEX_PANIC_MAX; }
void robot_setConvexPanicMax(byte cpm) { CONVEX_PANIC_MAX = cpm; }
