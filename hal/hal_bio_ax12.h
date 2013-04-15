/*
 * @file hal_ax12.h
 */

#ifndef _HAL_BIO_AX12_H_
#define _HAL_BIO_AX12_H_

#define AX12_DIR_FORWARD  8
#define AX12_DIR_BACK     2
#define AX12_DIR_LEFT     4
#define AX12_DIR_RIGHT    6

#define AX12_CW  8
#define AX12_CCW 2

// ERROR MASKS
#define ERR_INST     BIT6
#define ERR_OVERLOAD BIT5
#define ERR_CHECKSUM BIT4
#define ERR_RANGE    BIT3
#define ERR_OVERHEAT BIT2
#define ERR_ANG_LIM  BIT1
#define ERR_IN_VOLT  BIT0
#define ERR_NONE     0x00

void halBioAX12_initialize();

int halBioAX12_ping(int id);
void halBioAX12_act(int id);

int halBioAX12_getTemperature(int id);
int halBioAX12_getVoltage(int id);

int halBioAX12_setLed(int id, int state);
int halBioAX12_enableEndlessTurn(int id);

#endif
