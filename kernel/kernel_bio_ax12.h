/*
 * @file kernel_bio_ax12.h
 */

#ifndef _KERNEL_BIO_AX12_H_
#define _KERNEL_BIO_AX12_H_

#define AX12_CW  8
#define AX12_CCW 2

int kerBioAX12_ping(int id);
int kerBioAX12_act(int id);

int kerBioAX12_setLed(int id, int state);
int kerBioAX12_setMovingSpeed(int id, int speed, int direction);
int kerBioAX12_enableEndlessTurn(int id);

void kerBioAX12_registerInstruction();

int kerBioAX12_getVoltage(int id);
int kerBioAX12_getTemperature(int id);
int kerBioAX12_getPresentSpeed(int id);
int kerBioAX12_getPresentPosition(int id);

#endif
