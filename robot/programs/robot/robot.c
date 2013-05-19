/*
 * @file robot.c
 */

#include <hal.h>
#include <kernel.h>

#include "robot.h"
#include "../../lib/motor.h"

typedef enum { IDLE, ALIGN, MOVE } ROBOT_STATE;

static SENSOR_DATA sensorData;
static ROBOT_STATE robotState;

static int killProgram = FALSE;
static int updatedSensorData = FALSE;

static byte INTERVAL = 50;   // Number of milliseconds to wait before updating again the robot state
static byte THRESHOLD_A = 0;
static byte THRESHOLD_B = 0;
static byte THRESHOLD_C = 0;
static byte CONVEX_PANIC_MAX = 0;

static byte convexPanicCnt = 0;

static void onProgramStart()
{
    //halLcdShutDownBackLight();
    //halLcdShutDown();

    halTimer_b_enableInterruptCCR0();
    halTimer_a1_enableInterruptCCR0();

    halBioCom_initialize();

    updatedSensorData = FALSE;
    killProgram = FALSE;

    sensorData.left = 0;
    sensorData.right = 0;
    sensorData.center = 0;

    convexPanicCnt = CONVEX_PANIC_MAX;
    robotState = IDLE;

    TB0CCR0 = 32 * INTERVAL;
}

static int inRange(byte pointFixed, byte point, byte radius)
{
    return point > ( pointFixed - radius ) && point < ( pointFixed + radius );
}

static void onProgramUpdate()
{
    F_PTR action;

    if ( killProgram )
    {
        kerMenu_exitProgram();
    }
    else if ( updatedSensorData )
    {
        if ( robotState == ALIGN )
        {
            // Alignment logic
        }

        if ( robotState == MOVE )
        {
            // CW Movement logic
            action = &motor_advance;

            if ( inRange(THRESHOLD_B, sensorData.left, 3) )
            {
                if ( sensorData.center < THRESHOLD_C )
                    action = &motor_turnRight;
            }
            else
            {
                convexPanicCnt++;
                if ( convexPanicCnt > CONVEX_PANIC_MAX )
                {
                    action = &motor_turnLeft;
                    convexPanicCnt = 0;
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
        case BUTTON_S1:
            if ( robotState == IDLE )
            {
                robotState = ALIGN;
            }
            else
            {
                robotState = IDLE;
                motor_stop();
            }
            break;
        case BUTTON_S2:
            killProgram = TRUE;
            break;
    }
}

static void onTimerB0Interruption()
{
    SENSOR_DATA newSensorData;

    newSensorData = kerBioAXS1_getIR(100);
    sensorData.left = newSensorData.left;
    sensorData.right = newSensorData.right;
    sensorData.center = newSensorData.center;

    updatedSensorData = TRUE;
}

void robot_bootstrap()
{
    kerMenu_registerProgram("robot", &onProgramStart,
                &onProgramUpdate, &onProgramStop,
                &onButtonPressed, &halBioCom_isr_timer_update,
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
