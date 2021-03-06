/*
 * @file motor.h
 */

#ifndef _MOTOR_H_
#define _MOTOR_H_

void motor_setSpeed(unsigned int speed);
unsigned int motor_getSpeed();

void motor_stop();

void motor_advance();
void motor_retreat();

void motor_turnLeft();
void motor_turnRight();

void motor_moveLeft();
void motor_moveRight();

#endif
