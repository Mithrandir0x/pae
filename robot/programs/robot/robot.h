/*
 * @file robot.h
 */

#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <hal.h>

void robot_bootstrap();

byte robot_getUpdateInterval();
void robot_setUpdateInterval(byte t);

byte robot_getThresholdLeft();
void robot_setThresholdLeft(byte a);

byte robot_getThresholdRight();
void robot_setThresholdRight(byte b);

byte robot_getThresholdLeftAlt();
void robot_setThresholdLeftAlt(byte c);

byte robot_getConvexPanicMax();
void robot_setConvexPanicMax(byte cpm);

#endif
