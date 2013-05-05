/*
 * @file kernel_bio_ax12.c
 */

#include <hal_bio_com.h>
#include "kernel_bio_ax12.h"

// EEPROM AREA
#define MEM_MODEL_L       0x00 // Model Number (Low)
#define MEM_MODEL_H       0x01 // Model Number (High)
#define MEM_VFIRM         0x02 // Firmware Version
#define MEM_ID            0x03 // Actuator Identifier
#define MEM_BAUD_RATE     0x04 // Baud Rate
#define MEM_RDT           0x05 // Return Delay Time
#define MEM_CW_ANG_LIM_L  0x06 // Clockwise Angle Limit (Low)
#define MEM_CW_ANG_LIM_H  0x07 // Clockwise Angle Limit (High)
#define MEM_CCW_ANG_LIM_L 0x08 // Counter-Clockwise Angle Limit (Low)
#define MEM_CCW_ANG_LIM_H 0x09 // Counter-Clockwise Angle Limit (High)
#define MEM_TEMP_HIGH_LIM 0x0B // Highest Limit Temperature
#define MEM_TEMP_LOW_LIM  0x0C // Lowest Limit Temperature
#define MEM_VOLT_HIGH_LIM 0x0D // Highest Limit Voltage
#define MEM_MAX_TORQUE_L  0x0E // Max Torque (Low)
#define MEM_MAX_TORQUE_H  0x0F // Max Torque (High)
#define MEM_SRL           0x10 // Status Return Level
#define MEM_ALARM_LED     0x11 // Alarm LED
#define MEM_ALARM_SHTDWN  0x12 // Alarm Shutdown
#define MEM_DOWN_CALIB_L  0x14 // Down Calibration (Low)
#define MEM_DOWN_CALIB_H  0x15 // Down Calibration (High)
#define MEM_UP_CALIB_L    0x16 // Up Calibration (Low)
#define MEM_UP_CALIB_H    0x17 // Up Calibration (High)

// RAM AREA
#define MEM_TORQUE        0x18 // Torque Enable
#define MEM_LED           0x19 // LED
#define MEM_CW_CMP_MARGN  0x1A // Clockwise Compliance Margin
#define MEM_CCW_CMP_MARGN 0x1B // Counter-Clockwise Compliance Margin
#define MEM_CW_CMP_SLOPE  0x1C // Clockwise Compliance Slope
#define MEM_CCW_CMP_SLOPE 0x1D // Counter-Clockwise Compliance Slope
#define MEM_GOAL_POS_L    0x1E // Goal Position (Low)
#define MEM_GOAL_POS_H    0x1F // Goal Position (High)
#define MEM_MOV_SPEED_L   0x20 // Moving Speed (Low)
#define MEM_MOV_SPEED_H   0x21 // Moving Speed (High)
#define MEM_TORQUE_LIM_L  0x22 // Torque Limit (Low)
#define MEM_TORQUE_LIM_H  0x23 // Torque Limit (High)
#define MEM_PRES_POS_L    0x24 // Present Position (Low)
#define MEM_PRES_POS_H    0x25 // Present Position (High)
#define MEM_PRES_SPEED_L  0x26 // Present Speed (Low)
#define MEM_PRES_SPEED_H  0x27 // Present Speed (High)
#define MEM_PRES_LOAD_L   0x28 // Present Load (Low)
#define MEM_PRES_LOAD_H   0x29 // Present Load (High)
#define MEM_PRES_VOLT     0x2A // Present Voltage
#define MEM_PRES_TEMP     0x2B // Present Temperature
#define MEM_REG_INST      0x2C // Registered Instruction
#define MEM_MOVING        0x2E // Moving
#define MEM_LOCK          0x2F // Lock
#define MEM_PUNCH_L       0x30 // Punch (Low)
#define MEM_PUNCH_H       0x31 // Punch (High)

/**
 * The PING instruction of the AX-12.
 *
 * @param id The identifier of the actuator.
 * @return The error byte from the status packet or ERROR.
 */
int kerBioAX12_ping(int id)
{
    halBioCom_setInstruction(INS_PING);

    return halBioCom_transmit(id);
}

int kerBioAX12_act(int id)
{
    halBioCom_setInstruction(INS_ACTION);
    return halBioCom_transmit(id);
}

/**
 * Set either ON or OFF the LED from an actuator.
 *
 * (WRITE_DATA INSTRUCTION)
 *
 * @param id The identifier of the actuator.
 * @param state The state which the led should be, either ON or OFF.
 * @return The error byte from the status packet or ERROR.
 */
int kerBioAX12_setLed(int id, int state)
{
    halBioCom_setInstruction(INS_WRITE_DATA);
    halBioCom_addParameter(MEM_LED);

    if ( state == OFF )
        halBioCom_addParameter(0);
    else
        halBioCom_addParameter(1);

    return halBioCom_transmit(id);
}

/**
 * Set the speed and turn direction of the motor.
 *
 * (WRITE_DATA INSTRUCTION)
 *
 * @param id The identifier of the actuator.
 * @param speed
 * @param direction Set the turn direction either clockwise (AX12_CW) or counter-clockwise (AX12_CCW)
 */
int kerBioAX12_setMovingSpeed(int id, int speed, int direction)
{
    byte hi = 0x00;
    byte lo = 0x00;

    lo |= ( speed & 0x00ff );
    hi |= ( ( speed & 0x0300 ) >> 8 );

    if ( direction == AX12_CW )
        hi |= 0x04;

    halBioCom_setInstruction(INS_WRITE_DATA);
    halBioCom_addParameter(MEM_MOV_SPEED_L);
    halBioCom_addParameter(lo); // MEM_MOV_SPEED_L
    halBioCom_addParameter(hi); // MEM_MOV_SPEED_H

    return halBioCom_transmit(id);
}

/**
 * Set the actuator to endless turn mode. This allows to have a
 * continuosly rotating wheel.
 *
 * (WRITE_DATA INSTRUCTION)
 *
 * @param id The identifier of the actuator.
 * @return The error byte from the status packet or ERROR.
 */
int kerBioAX12_enableEndlessTurn(int id)
{
    halBioCom_setInstruction(INS_WRITE_DATA);

    halBioCom_addParameter(MEM_CW_ANG_LIM_L);
    halBioCom_addParameter(0x00); // MEM_CW_ANG_LIM_L
    halBioCom_addParameter(0x00); // MEM_CW_ANG_LIM_H
    halBioCom_addParameter(0x00); // MEM_CCW_ANG_LIM_L
    halBioCom_addParameter(0x00); // MEM_CCW_ANG_LIM_H

    return halBioCom_transmit(id);
}

/**
 * Returns the current voltage used by the actuator.
 *
 * @param id The identifier of the actuator.
 * @return The voltage value used by the actuator or ERROR.
 */
int kerBioAX12_getVoltage(int id)
{
    int result;
    byte* rx = halBioCom_getRX();

    halBioCom_setInstruction(INS_READ_DATA);
    halBioCom_addParameter(MEM_PRES_VOLT);
    halBioCom_addParameter(0x01);

    result = halBioCom_transmit(id);

    if ( result != ERROR && rx[4] == ERR_NONE )
        return ( rx[5] / 10 ); // Spec says that voltage is 10 times its value.

    return result;
}

/**
 * Returns the current temperature in celsius degrees of the actuator.
 *
 * @param id The identifier of the actuator.
 * @return The temperature in celsius of the actuator or ERROR.
 */
int kerBioAX12_getTemperature(int id)
{
    int result;
    byte* rx = halBioCom_getRX();

    halBioCom_setInstruction(INS_READ_DATA);
    halBioCom_addParameter(MEM_PRES_TEMP);
    halBioCom_addParameter(0x01);

    result = halBioCom_transmit(id);

    if ( result != ERROR && rx[4] == ERR_NONE )
        return rx[5];

    return result;
}

/**
 * Returns the current angular velocity of the actuator.
 *
 * @param id The identifier of the actuator.
 * @return The current angular velocity.
 */
int kerBioAX12_getPresentSpeed(int id)
{
    int result;
    byte* rx = halBioCom_getRX();

    halBioCom_setInstruction(INS_READ_DATA);
    halBioCom_addParameter(MEM_PRES_SPEED_L);
    halBioCom_addParameter(0x02);

    result = halBioCom_transmit(id);

    if ( result != ERROR && rx[4] == ERR_NONE )
    {
        // rx[5] MEM_PRES_SPEED_L
        // rx[6] MEM_PRES_SPEED_H
        return ( rx[5] | ( rx[6] << 8 ) );
    }

    return result;
}

