/*
 * @file robot.h
 */

#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <hal.h>

void robot_bootstrap();

byte robot_getUpdateInterval();
void robot_setUpdateInterval(byte t);

byte robot_getThresholdA();
void robot_setThresholdA(byte a);

byte robot_getThresholdB();
void robot_setThresholdB(byte b);

byte robot_getThresholdC();
void robot_setThresholdC(byte c);

byte robot_getConvexPanicMax();
void robot_setConvexPanicMax(byte cpm);

byte robot_getSpeed();
void robot_setSpeed(byte sp);

#endif
