/*
 * @file kernel_bio_axs1.h
 */

#ifndef _KERNEL_BIO_AXS1_H_
#define _KERNEL_BIO_AXS1_H_

#include <hal_common.h>

#define AXS1_OBSTACLE_DETECTED_LEFT   BIT2
#define AXS1_OBSTACLE_DETECTED_CENTER BIT1
#define AXS1_OBSTACLE_DETECTED_RIGHT  BIT0

typedef struct {
    byte left;
    byte center;
    byte right;
} SENSOR_DATA;

SENSOR_DATA kerBioAXS1_getIR(byte id);
SENSOR_DATA kerBioAXS1_getLuminosity(byte id);

int kerBioAXS1_getObstacleDetectionFlag(byte id);
int kerBioAXS1_getLuminosityFlag(byte id);

#endif
