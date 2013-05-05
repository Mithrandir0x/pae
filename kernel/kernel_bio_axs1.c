/*
 * @file kernel_bio_axs1.c
 */

#include <hal_common.h>
#include <hal_bio_com.h>
#include "kernel_bio_axs1.h"

// EEPROM AREA
#define MEM_MODEL_L       0x00 // Model Number (Low)
#define MEM_MODEL_H       0x01 // Model Number (High)
#define MEM_VFIRM         0x02 // Firmware Version
#define MEM_ID            0x03 // Actuator Identifier
#define MEM_BAUD_RATE     0x04 // Baud Rate
#define MEM_RDT           0x05 // Return Delay Time
#define MEM_H_L_TEMP      0x0B // Highest Limit Temperature
#define MEM_L_L_VOLT      0x0C // Lowest Limit Voltage
#define MEM_H_L_VOLT      0x0D // Highest Limit Voltage
#define MEM_SRL           0x10 // Status Return Level
#define MEM_ODC_DEFAULT   0x14 // Obstacle Detected Compare Value (Default Value)
#define MEM_LDC_DEFAULT   0x15 // Light Detected Compare Value (Default Value)

// RAM AREA
#define MEM_IR_LEFT       0x1A // Left IR Sensor Data
#define MEM_IR_CENTER     0x1B // Center IR Sensor Data
#define MEM_IR_RIGHT      0x1C // Right IR Sensor Data
#define MEM_LUM_LEFT      0x1D // Left Luminosity
#define MEM_LUM_CENTER    0x1E // Center Luminosity
#define MEM_LUM_RIGHT     0x1F // Right Luminosity
#define MEM_OD_FLAG       0x20 // Obstacle Detection Flag
#define MEM_LUM_FLAG      0x21 // Luminosity Flag
#define MEM_SOUND         0x23 // Sound Data
#define MEM_SND_MAX_HOLD  0x24 // Sound Data Max Hold
#define MEM_SND_COUNT     0x25 // Sound Detected Count
#define MEM_SND_TIME_L    0x26 // Sound Detected Time (Low)
#define MEM_SND_TIME_H    0x27 // Sound Detected Time (High)
#define MEM_BUZZER        0x28 // Buzzer Index
#define MEM_BUZZER_TIME   0x29 // Buzzer Time
#define MEM_PRES_VOLT     0x2A // Present Voltage
#define MEM_PRES_TEMP     0x2B // Present Temperature
#define MEM_REG_INST      0x2C // Registered Instruction
#define MEM_IRR_ARRIVED   0x2E // IR Remocon Arrived
#define MEM_IRR_RX_L      0x30 // IR Remocon RX Data (Low)
#define MEM_IRR_RX_H      0x31 // IR Remocon RX Data (High)
#define MEM_IRR_TX_L      0x32 // IR Remocon TX Data (Low)
#define MEM_IRR_TX_H      0x33 // IR Remocon TX Data (High)
#define MEM_ODC           0x34 // Obstacle Detected Compare Value
#define MEM_LDC           0x35 // Light Detected Compare Value

SENSOR_DATA kerBioAXS1_getIR(byte id)
{
    int result;
    byte* rx = halBioCom_getRX();
    SENSOR_DATA data;

    data.left = 0;
    data.center = 0;
    data.right = 0;

    halBioCom_setInstruction(INS_READ_DATA);
    halBioCom_addParameter(MEM_IR_LEFT);
    halBioCom_addParameter(0x03);

    result = halBioCom_transmit(id);

    if ( result != ERROR && rx[4] == ERR_NONE )
    {
        data.left = rx[5];
        data.center = rx[6];
        data.right = rx[7];
    }

    return data;
}

SENSOR_DATA kerBioAXS1_getLuminosity(byte id)
{
    int result;
    byte* rx = halBioCom_getRX();
    SENSOR_DATA data;

    data.left = 0;
    data.center = 0;
    data.right = 0;

    halBioCom_setInstruction(INS_READ_DATA);
    halBioCom_addParameter(MEM_LUM_LEFT);
    halBioCom_addParameter(0x03);

    result = halBioCom_transmit(id);

    if ( result != ERROR && rx[4] == ERR_NONE )
    {
        data.left = rx[5];
        data.center = rx[6];
        data.right = rx[7];
    }

    return data;
}

int kerBioAXS1_getObstacleDetectionFlag(byte id)
{
    int result;
    byte* rx = halBioCom_getRX();

    halBioCom_setInstruction(INS_READ_DATA);
    halBioCom_addParameter(MEM_OD_FLAG);
    halBioCom_addParameter(0x01);

    result = halBioCom_transmit(id);

    if ( result != ERROR && rx[4] == ERR_NONE )
        return rx[5];

    return result;
}

int kerBioAXS1_getLuminosityFlag(byte id)
{
    int result;
    byte* rx = halBioCom_getRX();

    halBioCom_setInstruction(INS_READ_DATA);
    halBioCom_addParameter(MEM_LUM_FLAG);
    halBioCom_addParameter(0x01);

    result = halBioCom_transmit(id);

    if ( result != ERROR && rx[4] == ERR_NONE )
        return rx[5];

    return result;
}

