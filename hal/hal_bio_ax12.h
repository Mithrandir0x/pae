/*
 * @file hal_ax12.h
 */

#ifndef _HAL_BIO_AX12_H_
#define _HAL_BIO_AX12_H_

#define TRX_BUFFER_SIZE 32

typedef unsigned char byte;

typedef struct {
    byte error;
    byte instruction;
    byte* parameters;
    byte length;
    byte checksum;
} AX_12_PACKET;

void halBioAX12_initialize();

// Instructions
void halBioAX12_ping(int id);
void halBioAX12_setLed(int id, int state);

#endif