/*
 * @file robot.c
 */

#include <stdio.h>
#include <hal.h>
#include <kernel.h>

#include "robot.h"
#include "../../lib/motor.h"

typedef enum { IDLE, ALIGN, MOVE } ROBOT_STATE;

static SENSOR_DATA sensorData;
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
static byte THRESHOLD_A = 0;
static byte THRESHOLD_B = 20;
static byte THRESHOLD_C = 255;
static byte CONVEX_PANIC_MAX = 20;

static byte convexPanicCnt = 0;
static byte concavePanicCnt = 0;
static byte nearObstacleCnt = 0;

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

    sensorData.left = 0;
    sensorData.right = 0;
    sensorData.center = 0;

    convexPanicCnt = 0;
    concavePanicCnt = 0;
    nearObstacleCnt = 0;

    motor_setSpeed(256);

    convexPanicCnt = CONVEX_PANIC_MAX;
    robotState = IDLE;

    TB0CCR0 = 32 * INTERVAL;
}

static int inRange(byte pointFixed, byte point, byte radius)
{
    return point >= ( pointFixed - radius ) && point <= ( pointFixed + radius );
}

static int atLeast(byte pointFixed, byte point)
{
    return point >= pointFixed;
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

        sensorData.left = data.left;
        sensorData.right = data.right;
        sensorData.center = data.center;

        sprintf(__lcd_buffer, "  LEFT: %03d", data.left);
        halLcdPrintLineCol(__lcd_buffer, 3, 1, OVERWRITE_TEXT);

        sprintf(__lcd_buffer, "CENTER: %03d", data.center);
        halLcdPrintLineCol(__lcd_buffer, 4, 1, OVERWRITE_TEXT);

        sprintf(__lcd_buffer, " RIGHT: %03d", data.right);
        halLcdPrintLineCol(__lcd_buffer, 5, 1, OVERWRITE_TEXT);

        /* if ( inRange(THRESHOLD_B, sensorData.left, 30) )
        {
            halLcdPrintLine("IN RANGE    ", 7, OVERWRITE_TEXT | INVERT_TEXT);
        }
        else
        {
            halLcdPrintLine("OUT OF RANGE", 7, OVERWRITE_TEXT | INVERT_TEXT);
        } */

        if ( robotState == ALIGN )
        {
            // Alignment logic
        }

        if ( robotState == MOVE )
        {
            // CW Movement logic
            motor_setSpeed(256);
            action = &motor_advance;

            if ( data.center >= 255 || concavePanicCnt > 0 )
            {
                if ( nearObstacleCnt < 30 )
                {
                    nearObstacleCnt++;
                }
                else
                {
                    if ( concavePanicCnt < 27 )
                    {
                        concavePanicCnt++;
                        motor_setSpeed(512);
                        action = &motor_turnRight;
                    }
                    else
                    {
                        concavePanicCnt = 0;
                        nearObstacleCnt = 0;
                    }
                }
            }

            //if ( inRange(THRESHOLD_B, sensorData.left, 30) )
            //if ( atLeast(THRESHOLD_B, sensorData.left) )
            if ( data.left > 150 )
            {
                convexPanicCnt = 0;
                //if ( sensorData.center > THRESHOLD_C )
            }
            else
            {
                convexPanicCnt++;
                if ( convexPanicCnt >= 95 )
                {
                    motor_setSpeed(512);
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

byte robot_getThresholdA() { return THRESHOLD_A; }
void robot_setThresholdA(byte a) { THRESHOLD_A = a; }

byte robot_getThresholdB() { return THRESHOLD_B; }
void robot_setThresholdB(byte b) { THRESHOLD_B = b; }

byte robot_getThresholdC() { return THRESHOLD_C; }
void robot_setThresholdC(byte c) { THRESHOLD_C = c; }

byte robot_getConvexPanicMax() { return CONVEX_PANIC_MAX; }
void robot_setConvexPanicMax(byte cpm) { CONVEX_PANIC_MAX = cpm; }

byte robot_getSpeed() { return motor_getSpeed(); }
void robot_setSpeed(byte sp) { motor_setSpeed(sp); }
