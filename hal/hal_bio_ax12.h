/*
 * @file hal_ax12.h
 */

#ifndef _HAL_BIO_AX12_H_
#define _HAL_BIO_AX12_H_

void halBioAX12_initialize();

int halBioAX12_ping(int id);
void halBioAX12_act(int id);

int halBioAX12_getTemperature(int id);
int halBioAX12_getVoltage(int id);

int halBioAX12_setLed(int id, int state);
int halBioAX12_enableEndlessTurn(int id);

#endif
