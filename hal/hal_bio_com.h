/*
 * @file hal_bio_com.h
 */

#ifndef _HAL_BIO_COM_H_
#define _HAL_BIO_COM_H_

#include "hal_common.h"

#define AX12_BROADCAST_ID 0xFE

#define ERROR -1

// INSTRUCTION IDs
#define INS_NONE       0x00
#define INS_PING       0x01
#define INS_READ_DATA  0x02
#define INS_WRITE_DATA 0x03
#define INS_REG_WRITE  0x04
#define INS_ACTION     0x05
#define INS_SYNC_WRITE 0x83

// ERROR MASKS
#define ERR_INST     BIT6
#define ERR_OVERLOAD BIT5
#define ERR_CHECKSUM BIT4
#define ERR_RANGE    BIT3
#define ERR_OVERHEAT BIT2
#define ERR_ANG_LIM  BIT1
#define ERR_IN_VOLT  BIT0
#define ERR_NONE     0x00

void halBioCom_initialize();
void halBioCom_shutdown();

int halBioCom_addParameter(byte parameter);
void halBioCom_setInstruction(byte instruction);
void halBioCom_registerInstruction();
int halBioCom_transmit(byte id);

byte* halBioCom_getTX();
byte* halBioCom_getRX();

void halBioCom_isr_timer_update();

#endif
