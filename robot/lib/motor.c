/*
 * @file motor.c
 */

#include <kernel.h>

unsigned int __motor_internalSpeed = 0;
int __motor_flag = 0;

void motor_setSpeed(unsigned int speed)
{
    __motor_internalSpeed = speed;
}

void motor_stop()
{
#ifndef __MOTOR_DEBUG_MODE
    kerBioAX12_setMovingSpeed(1, 0, AX12_CW);
    kerBioAX12_setMovingSpeed(2, 0, AX12_CW);
    kerBioAX12_setMovingSpeed(3, 0, AX12_CCW);
    kerBioAX12_setMovingSpeed(4, 0, AX12_CCW);
#endif

#ifdef __MOTOR_DEBUG_MODE
    halLed_rx_setLed(LED_RX_ALL, OFF);

    __motor_flag = kerBioAX12_setMovingSpeed(1, 0, AX12_CW);
    halLed_rx_setLed(LED_R1, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(2, 0, AX12_CW);
    halLed_rx_setLed(LED_R2, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(3, 0, AX12_CCW);
    halLed_rx_setLed(LED_R3, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(4, 0, AX12_CCW);
    halLed_rx_setLed(LED_R4, __motor_flag);
#endif
}

void motor_advance()
{
#ifndef __MOTOR_DEBUG_MODE
    kerBioAX12_setMovingSpeed(1, __motor_internalSpeed, AX12_CW);
    kerBioAX12_setMovingSpeed(2, __motor_internalSpeed, AX12_CW);
    kerBioAX12_setMovingSpeed(3, __motor_internalSpeed, AX12_CCW);
    kerBioAX12_setMovingSpeed(4, __motor_internalSpeed, AX12_CCW);
#endif

#ifdef __MOTOR_DEBUG_MODE
    halLed_rx_setLed(LED_RX_ALL, OFF);

    __motor_flag = kerBioAX12_setMovingSpeed(1, __motor_internalSpeed, AX12_CW);
    halLed_rx_setLed(LED_R1, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(2, __motor_internalSpeed, AX12_CW);
    halLed_rx_setLed(LED_R2, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(3, __motor_internalSpeed, AX12_CCW);
    halLed_rx_setLed(LED_R3, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(4, __motor_internalSpeed, AX12_CCW);
    halLed_rx_setLed(LED_R4, __motor_flag);
#endif
}

void motor_retreat()
{
#ifndef __MOTOR_DEBUG_MODE
    kerBioAX12_setMovingSpeed(1, __motor_internalSpeed, AX12_CCW);
    kerBioAX12_setMovingSpeed(2, __motor_internalSpeed, AX12_CCW);
    kerBioAX12_setMovingSpeed(3, __motor_internalSpeed, AX12_CW);
    kerBioAX12_setMovingSpeed(4, __motor_internalSpeed, AX12_CW);
#endif

#ifdef __MOTOR_DEBUG_MODE
    halLed_rx_setLed(LED_RX_ALL, OFF);

    __motor_flag = kerBioAX12_setMovingSpeed(1, __motor_internalSpeed, AX12_CCW);
    halLed_rx_setLed(LED_R1, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(2, __motor_internalSpeed, AX12_CCW);
    halLed_rx_setLed(LED_R2, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(3, __motor_internalSpeed, AX12_CW);
    halLed_rx_setLed(LED_R3, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(4, __motor_internalSpeed, AX12_CW);
    halLed_rx_setLed(LED_R4, __motor_flag);
#endif
}

void motor_turnLeft()
{
#ifndef __MOTOR_DEBUG_MODE
    kerBioAX12_setMovingSpeed(1, __motor_internalSpeed, AX12_CW);
    kerBioAX12_setMovingSpeed(2, __motor_internalSpeed, AX12_CW);
    kerBioAX12_setMovingSpeed(3, __motor_internalSpeed, AX12_CW);
    kerBioAX12_setMovingSpeed(4, __motor_internalSpeed, AX12_CW);
#endif

#ifdef __MOTOR_DEBUG_MODE
    halLed_rx_setLed(LED_RX_ALL, OFF);

    __motor_flag = kerBioAX12_setMovingSpeed(1, __motor_internalSpeed, AX12_CW);
    halLed_rx_setLed(LED_R1, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(2, __motor_internalSpeed, AX12_CW);
    halLed_rx_setLed(LED_R2, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(3, __motor_internalSpeed, AX12_CW);
    halLed_rx_setLed(LED_R3, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(4, __motor_internalSpeed, AX12_CW);
    halLed_rx_setLed(LED_R4, __motor_flag);
#endif
}

void motor_turnRight()
{
#ifndef __MOTOR_DEBUG_MODE
    kerBioAX12_setMovingSpeed(1, __motor_internalSpeed, AX12_CCW);
    kerBioAX12_setMovingSpeed(2, __motor_internalSpeed, AX12_CCW);
    kerBioAX12_setMovingSpeed(3, __motor_internalSpeed, AX12_CCW);
    kerBioAX12_setMovingSpeed(4, __motor_internalSpeed, AX12_CCW);
#endif

#ifdef __MOTOR_DEBUG_MODE
    halLed_rx_setLed(LED_RX_ALL, OFF);

    __motor_flag = kerBioAX12_setMovingSpeed(1, __motor_internalSpeed, AX12_CCW);
    halLed_rx_setLed(LED_R1, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(2, __motor_internalSpeed, AX12_CCW);
    halLed_rx_setLed(LED_R2, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(3, __motor_internalSpeed, AX12_CCW);
    halLed_rx_setLed(LED_R3, __motor_flag);

    __motor_flag = kerBioAX12_setMovingSpeed(4, __motor_internalSpeed, AX12_CCW);
    halLed_rx_setLed(LED_R4, __motor_flag);
#endif
}
