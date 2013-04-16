/*
 * @file hal_bio_ax12.c
 */

// http://support.robotis.com/en/product/dynamixel/ax_series/dxl_ax_actuator.htm

/**
 * Dynamixel AX-12 Packet Types:
 *  - INSTRUCTION
 *  - STATUS
 *
 * Instruction Packet Spec:
 *
 *      [0xFF] [0xFF] [ID] [LNG] [INST] [PA_1] [PA_2] ... [PA_M] [CHKSM]
 *        0      1     2     3     4     5      6          N-1     N
 *      [FF] [FF] [04] [02] [00] [FE]
 *        0    1    2    3    4    5
 *
 * Status Packet Spec:
 *
 *      [0xFF] [0xFF] [ID] [LNG] [ERR] [PA_1] [PA_2] ... [PA_M] [CHKSM]
 *
 *      FF FF 02 02 00 ~~
 */

#include <msp430x54xa.h>
#include "hal_common.h"
#include "hal_bio_ax12.h"
#include "hal_timer.h"

#define ERROR -1

#define INS_NONE       0x00
#define INS_PING       0x01
#define INS_READ_DATA  0x02
#define INS_WRITE_DATA 0x03
#define INS_REG_WRITE  0x04
#define INS_ACTION     0x05
#define INS_SYNC_WRITE 0x83

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

// MACROUTILS
#define SET_TX ( P3OUT |= BIT7 )
#define SET_RX ( P3OUT &= ~BIT7 )

#define RX_PACKET_LENGTH ( rx[3] )
#define RX_PACKET_STATUS ( rx[4] )
#define RX_PACKET_CHKSUM ( rx[RX_PACKET_LENGTH + 3] ) // 0xFF + 0xFF + ID

#define TX_PACKET_LENGTH      ( tx[3] )
#define TX_PACKET_INSTRUCTION ( tx[4] )

#define IS_RX_HEADER_SET ( rx[0] == 0xFF && rx[1] == 0xFF )

/**
 * This macro allows to surround blocks of code with a time-out mechanism. It's very
 * similar to the "__delay" method, but it works a bit different. Instead of making
 * the user-code to wait for the timer's interruption, it allows the user to define
 * its own code to be executed.
 *
 * @param _MS_ The quantity of microseconds to wait before leaving
 */
#define __TIMEOUT(_MS_) halTimer_a1_setCCRMicroTimedInterruption(TIMER_CCR0, _MS_); __TIMEOUT_STALL = TRUE; while( __TIMEOUT_STALL )

#define TRX_BUFFER_SIZE 32

// Packet Buffers
byte tx[TRX_BUFFER_SIZE];
byte rx[TRX_BUFFER_SIZE];

volatile int __STALL = FALSE;
volatile int __TIMEOUT_STALL = FALSE;

volatile int __BUFFER_OVERFLOW = FALSE;

volatile int __REGISTER_NEXT_INSTRUCTION = FALSE;

volatile int rx_index = -1;     // Index of the packet byte fed from

// Declaration of internal functions
inline void __enable_interruptions();
inline void __disable_interruptions();

// Interrupt Vector definitions
#pragma vector = USCI_A0_VECTOR
__interrupt void on_receive_byte()
{
    __disable_interruptions();

    if ( rx_index < TRX_BUFFER_SIZE )
    {
        rx[rx_index] = UCA0RXBUF;
        rx_index++;
    }
    else
    {
        __BUFFER_OVERFLOW = TRUE;
    }

    __enable_interruptions();
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void update_timeout_stall_state()
{
    halTimer_a1_disableInterruptCCR0();

    if ( __STALL )
        __STALL = FALSE;

    if ( __TIMEOUT_STALL )
    {
        __TIMEOUT_STALL = FALSE;
        halTimer_a1_setCCRMicroTimedInterruption(TIMER_CCR0, 0);
    }

    halTimer_a1_enableInterruptCCR0();
}


// Definition of some internal functions
inline void __disable_interruptions()
{
    UCA0IE &= ~UCRXIE;
}

inline void __enable_interruptions()
{
    UCA0IE |= UCRXIE;
}

/**
 * Internal function to stall the processor.
 *
 * @param ms The amount of microseconds to wait before executing anything else.
 */
void __delay(unsigned int ms)
{
    halTimer_a1_setCCRMicroTimedInterruption(TIMER_CCR0, ms);

    __STALL = TRUE;
    while ( __STALL );

    halTimer_a1_setCCRMicroTimedInterruption(TIMER_CCR0, 0);
}

/**
 * Fills the buffer passed by parameter with 0x00 bytes.
 *
 * @param buffer Pointer to a buffer to be cleared (either "tx" or "rx")
 */
void clearBuffer(byte *buffer)
{
    int i;
    for ( i = 0 ; i < TRX_BUFFER_SIZE ; i++ )
    {
        buffer[i] = 0x00;
    }
}

/**
 * Calculates the checksum for the current state of the transmit buffer.
 *
 * @return The checksum of the transmit buffer packet.
 */
byte checksum()
{
    int i;
    int n = 3 + TX_PACKET_LENGTH; // ID + LENGTH + INSTRUCTION + ( PARAM_1 + ··· + PARAM_N )
    byte checksum = 0;

    for ( i = 2 ; i < n ; i++ )
    {
        checksum += tx[i];
    }

    return ~checksum;
}

/**
 * Verifies that the checksum of the receive buffer is valid.
 *
 * @return FALSE if invalid checksum, otherwise any value.
 */
int validate_checksum()
{
    int i = 2;
    int n = 3 + RX_PACKET_LENGTH;
    byte checksum = 0;

    for ( ; i < n ; i++ )
    {
        checksum += rx[i];
    }

    return ( ~checksum == RX_PACKET_CHKSUM );
}

int receive()
{
    rx_index = 0;
    __BUFFER_OVERFLOW = FALSE;

    SET_RX;

    __TIMEOUT(300) // FIXME Verify timeout time when waiting for packet reception
    {
        if ( IS_RX_HEADER_SET )
        {
            if ( rx_index >= RX_PACKET_LENGTH + 4 ) // 0xFF + 0xFF + ID + CHKSM
            {
                if ( validate_checksum() )
                    return RX_PACKET_STATUS;

                break;
            }
        }
    }

    return ERROR;
}

void sendByte(byte b)
{
    while ( !(UCA0IFG & UCTXIFG) ); // Wait for transmit buffer to be ready
    UCA0TXBUF = b;  // Fill the transmit buffer
}

/**
 * This function is in charge of sending the transmit buffer to the
 * actuator selected by its ID.
 *
 * @param id Identifier of the actuator
 * @return ERROR if something has gone awry, otherwise, GOOD.
 */
int transmit(byte id)
{
    volatile unsigned int i = 0;
    volatile int result;
    int packet_size = 6 + TX_PACKET_LENGTH;

    SET_TX; // Set P3.7 as TRANSMIT

    tx[0] = 0xFF;                     // Incoming packet Header
    tx[1] = 0xFF;                     // Incoming packet Header
    tx[2] = id;                       // AX12 Actuator Identifier
    TX_PACKET_LENGTH += 2;            // Length of the packet to be sent
    tx[packet_size - 1] = checksum(); // Checksum

    for ( i = 0 ; i < packet_size ; i++ )
    {
        sendByte(tx[i]);
    }

    __delay(20);

    result = receive();

    SET_TX;

    return result;
}

/**
 * This function transmit the current state of the transmit buffer, but it
 * won't wait for any response.
 *
 * @param id Identifier of the actuator
 */
void lazy_transmit(byte id)
{
    volatile unsigned int i = 0;
    volatile int result;
    int packet_size = 6 + TX_PACKET_LENGTH;

    SET_TX; // Set P3.7 as TRANSMIT

    tx[0] = 0xFF;                     // Incoming packet Header
    tx[1] = 0xFF;                     // Incoming packet Header
    tx[2] = id;                       // AX12 Actuator Identifier
    TX_PACKET_LENGTH += 2;            // Length of the packet to be sent
    tx[packet_size - 1] = checksum(); // Checksum

    for ( i = 0 ; i < packet_size ; i++ )
    {
        sendByte(tx[i]);
    }

    __delay(20);
}

/**
 * Adds a new parameter to the transmit buffer.
 *
 * @param parameter The value assigned to the parameter array.
 * @return The index of the new parameter added, or ERROR if something has gone awry.
 */
int addParameter(byte parameter)
{
    int i = 5 + TX_PACKET_LENGTH;    // Get the index of the newest parameter to be added
    if ( i < TRX_BUFFER_SIZE - 1 )   // Check if the buffer allows anymore parameters
    {
        tx[i] = parameter;           // If so, set the parameter
        return ++TX_PACKET_LENGTH;   // and return the index of such parameter.
    }

    return ERROR;
}

/**
 * Internal method to set the instruction of the transmit buffer.
 *
 * @param inst Instruction to be set.
 */
inline void setInstruction(byte inst)
{
    clearBuffer(tx);
    clearBuffer(rx);

    if ( __REGISTER_NEXT_INSTRUCTION && inst == INS_WRITE_DATA )
    {
        TX_PACKET_INSTRUCTION = INS_REG_WRITE;
        __REGISTER_NEXT_INSTRUCTION = FALSE;
    }
    else
    {
        TX_PACKET_INSTRUCTION = inst;
    }
}

/**
 * Sets USCI to UART mode, and configures required registers
 * to allow communication with AX12 actuators.
 *
 * It uses Subsystem Master Clock (SMCLK) as source.
 *
 * WARNING!
 * -----------------------------------------------------------------
 * This API uses Timer A1 to manage certain state conditions. Do not
 * implement an ISR based on TIMER A1.
 * -----------------------------------------------------------------
 */
void halBioAX12_initialize()
{
    UCA0CTL1 |= UCSWRST; // Enable Software reset. USCI logic held in reset state

    UCA0CTL0 = 0; // Disable parity  // AX-12 Requirement
                  // Set odd parity
                  // LSB First
                  // 8-bit data      // AX-12 Requirement
                  // One stop bit    // AX-12 Requirement
                  // UART Mode (UCSMODEx = 0)
                  // Obviously, asynchronous mode // AX-12 Requirement
    UCA0CTL1 |= UCSSEL__SMCLK; // Set USCI Clock source (BRCLK) from SMCLK
    // UCA0BRW = 1;
    UCA0BR0 = 1; // Sets baud rate prescaler to 1 (This means BRCLK is divided by 1)
    UCA0BR1 = 0;
    UCA0MCTL = UCOS16; // Enable oversampling mode, but do not select any kind of modulation.

    // Up to here, the BRCLK is SMCLK/16.

    P3SEL |= ( BIT4 | BIT5 ); // Port lines P3.4 and P3.5 selected as default I/O function
        // P3.4 = UART0TX
        // P3.5 = UART0RX

    P3REN |= ( BIT4 | BIT5 ); // Enable "Pull-Up/Pull-Down" resistor for P3.4 and P3.5
    P3OUT |= ( BIT4 | BIT5 ); // Set initial state as "Pull-Up"

    // Dynamixel Motors are Half-Duplex, meaning that we can only receive from OR transmit to.
    // P3.7 is in charge of selecting the data direction.
    // As Page 8 of Bioloid AX-12 spec states, the DIRECTION_PORT bit is described as:
    //  HI for transmiting (Tx)
    //  LO for receiving (Rx)
    P3DIR |= BIT7; // Set P3.7 as OUTPUT (Used as Tx/Rx Selector (0/1))
    P3SEL &= ~BIT7; // Set P3.7 as GPIO
    P3OUT &= ~BIT7; // Set P3.7 initial value to 0 (i.e. the controller is expecting to receive data)

    UCA0CTL1 &= ~UCSWRST; // Disable Software reset.
    UCA0IE |= UCRXIE; // Enable Receive interruptions

    // Initialize timer a1. It will use SMCLK signal.
    halTimer_a1_initialize(TIMER_CLKSRC_SMCLK, TIMER_MODE_UP);

    __delay(100); // FIXME Verify delay time when waiting for port stabilization
}

/**
 * The PING instruction of the AX-12.
 *
 * @param id The identifier of the actuator.
 * @return The error byte from the status packet or ERROR.
 */
int halBioAX12_ping(int id)
{
    setInstruction(INS_PING);

    return transmit(id);
}

void halBioAX12_act(int id)
{
    setInstruction(INS_ACTION);
    lazy_transmit(id);
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
int halBioAX12_setLed(int id, int state)
{
    setInstruction(INS_WRITE_DATA);
    addParameter(MEM_LED);

    if ( state == OFF )
        addParameter(0);
    else
        addParameter(1);

    return transmit(id);
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
int halBioAX12_setMovingSpeed(int id, int speed, int direction)
{
    int hi = 0x00;
    int lo = 0x00;

    lo |= ( speed & 0x00ff );

    hi |= ( ( speed & 0x0300 ) >> 8 );
    if ( direction == AX12_CW )
        hi |= 0x0400;

    setInstruction(INS_WRITE_DATA);
    addParameter(MEM_MOV_SPEED_L);
    addParameter(lo); // MEM_MOV_SPEED_L
    addParameter(hi); // MEM_MOV_SPEED_H

    return transmit(id);
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
int halBioAX12_enableEndlessTurn(int id)
{
    setInstruction(INS_WRITE_DATA);

    addParameter(MEM_CW_ANG_LIM_L);
    addParameter(0x00); // MEM_CW_ANG_LIM_L
    addParameter(0x00); // MEM_CW_ANG_LIM_H
    addParameter(0x00); // MEM_CCW_ANG_LIM_L
    addParameter(0x00); // MEM_CCW_ANG_LIM_H

    return transmit(id);
}

/**
 * There are several public functions of the AX-12 noted as WRITE_DATA instruction.
 * When called, these functions will make the actuator to update its state immediately.
 *
 * But if you want to use the ACTION instruction and synchronize with every actuator,
 * for example, this function allows to indicate that the WRITE_DATA instruction should
 * be a REG_WRITE one.
 *
 * After a REG_WRITE instruction has been done, it will be required to call this function
 * again if you wish that the next function should be noted as a REG_WRITE.
 */
void halBioAX12_registerInstruction()
{
    __REGISTER_NEXT_INSTRUCTION = TRUE;
}

/**
 * Returns the current voltage used by the actuator.
 *
 * @param id The identifier of the actuator.
 * @return The voltage value used by the actuator or ERROR.
 */
int halBioAX12_getVoltage(int id)
{
    int result;

    setInstruction(INS_READ_DATA);
    addParameter(MEM_PRES_VOLT);
    addParameter(0x01);

    result = transmit(id);

    if ( result != ERROR && RX_PACKET_STATUS == ERR_NONE )
        return ( rx[5] / 10 ); // Spec says that voltage is 10 times its value.

    return result;
}

/**
 * Returns the current temperature in celsius degrees of the actuator.
 *
 * @param id The identifier of the actuator.
 * @return The temperature in celsius of the actuator or ERROR.
 */
int halBioAX12_getTemperature(int id)
{
    int result;

    setInstruction(INS_READ_DATA);
    addParameter(MEM_PRES_TEMP);
    addParameter(0x01);

    result = transmit(id);

    if ( result != ERROR && RX_PACKET_STATUS == ERR_NONE )
        return rx[5];

    return result;
}

/**
 * Returns the current angular velocity of the actuator.
 *
 * @param id The identifier of the actuator.
 * @return The current angular velocity.
 */
int halBioAX12_getPresentSpeed(int id)
{
    int result;

    setInstruction(INS_READ_DATA);
    addParameter(MEM_PRES_SPEED_L);
    addParameter(0x02);

    result = transmit(id);

    if ( result != ERROR && RX_PACKET_STATUS == ERR_NONE )
    {
        // rx[5] MEM_PRES_SPEED_L
        // rx[6] MEM_PRES_SPEED_H
        return ( rx[5] | ( rx[6] << 8 ) );
    }

    return result;
}
