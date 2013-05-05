/*
 * @file motor.h
 */

#ifndef _MOTOR_H_
#define _MOTOR_H_

void motor_setSpeed(unsigned int speed);
void motor_stop();

void motor_advance();
void motor_retreat();

void motor_turnLeft();
void motor_turnRight();

#endif
