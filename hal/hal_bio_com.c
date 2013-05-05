
#include <msp430x54xa.h>
#include "hal_common.h"
#include "hal_bio_com.h"

// MACROUTILS
#define SET_TX ( P3OUT |= BIT7 )
#define SET_RX ( P3OUT &= ~BIT7 )

#define RX_PACKET_LENGTH ( __rx[3] )
#define RX_PACKET_STATUS ( __rx[4] )
#define RX_PACKET_CHKSUM ( __rx[RX_PACKET_LENGTH + 3] ) // 0xFF + 0xFF + ID

#define TX_PACKET_LENGTH      ( __tx[3] )
#define TX_PACKET_INSTRUCTION ( __tx[4] )

#define IS_RX_HEADER_SET ( __rx[0] == 0xFF && __rx[1] == 0xFF )

#define TRX_BUFFER_SIZE 32

// Packet Buffers
byte __tx[TRX_BUFFER_SIZE];
byte __rx[TRX_BUFFER_SIZE];

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
        __rx[rx_index] = UCA0RXBUF;
        rx_index++;
    }
    else
    {
        __BUFFER_OVERFLOW = TRUE;
    }

    __enable_interruptions();
}

void halBioCom_isr_timer_update()
{
    if ( __STALL )
        __STALL = FALSE;

    if ( __TIMEOUT_STALL )
    {
        __TIMEOUT_STALL = FALSE;
        //halTimer_a1_setCCRMicroTimedInterruption(TIMER_CCR0, 0);
        TA1R = 0;
        TA1CCR0 = 0;
        //TA1CCTL0 &= ~BIT4;
    }
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
    __STALL = TRUE;
    //halTimer_a1_setCCRMicroTimedInterruption(TIMER_CCR0, ms);
    //halTimer_a1_enableInterruptCCR0();
    //TA1CCTL0 |= BIT4;
    TA1R = 0;
    TA1CCR0 = 16 * ms;

    while ( __STALL );

    //halTimer_a1_disableInterruptCCR0();
    //TA1CCTL0 &= ~BIT4;
    //halTimer_a1_setCCRMicroTimedInterruption(TIMER_CCR0, 0);
    TA1R = 0;
    TA1CCR0 = 0;
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
        checksum += __tx[i];
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
        checksum += __rx[i];
    }

    checksum = ~checksum;

    return ( checksum == RX_PACKET_CHKSUM );
}

/**
 * This function is in charge of status packet reception, after an instruction
 * packet has been sent by function "transmit".
 *
 * DO NOT INVOKE THIS FUNCTION IF YOU'RE NOT EXPECTING TO RECEIVE ANYTHING!
 *
 * @return ERROR if timeout, ERR_CHECKSUM if packet received but checksum failed,
 *         otherwise status packet error byte.
 */
int receive()
{
    volatile int comparison_checksum = FALSE;

    rx_index = 0;
    __BUFFER_OVERFLOW = FALSE;

    SET_RX;

    //TA1CCR0 = 16000;
    //TA1CCTL0 |= BIT4;
    //halTimer_a1_setCCRMicroTimedInterruption(TIMER_CCR0, 1000);
    //halTimer_a1_enableInterruptCCR0();
    //TA1CCTL0 |= BIT4;
    __TIMEOUT_STALL = TRUE;
    TA1R = 0;
    TA1CCR0 = 16 * 1000;
    while( __TIMEOUT_STALL )
    {
        if ( IS_RX_HEADER_SET )
        {
            if ( rx_index >= RX_PACKET_LENGTH + 4 ) // 0xFF + 0xFF + ID + CHKSM
            {
                //TA1CCTL0 &= ~BIT4;
                //TA1CCR0 = 0;
                //halTimer_a1_disableInterruptCCR0();
                //halTimer_a1_setCCRMicroTimedInterruption(TIMER_CCR0, 0);
                TA1R = 0;
                TA1CCR0 = 0;
                //TA1CCTL0 &= ~BIT4;

                comparison_checksum = validate_checksum();

                if ( comparison_checksum )
                    return RX_PACKET_STATUS;

                return ERR_CHECKSUM;
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
 * @return ERROR if timeout, ERR_CHECKSUM if packet received but checksum failed,
 *         otherwise status packet error byte.
 */
int halBioCom_transmit(byte id)
{
    volatile unsigned int i = 0;
    volatile int result = 0;
    int packet_size = 6 + TX_PACKET_LENGTH;

    SET_TX; // Set P3.7 as TRANSMIT

    __tx[0] = 0xFF;                     // Incoming packet Header
    __tx[1] = 0xFF;                     // Incoming packet Header
    __tx[2] = id;                       // AX12 Actuator Identifier
    TX_PACKET_LENGTH += 2;            // Length of the packet to be sent
    __tx[packet_size - 1] = checksum(); // Checksum

    for ( i = 0 ; i < packet_size ; i++ )
    {
        sendByte(__tx[i]);
    }

    __delay(250);

    if ( id != AX12_BROADCAST_ID )
        result = receive();

    SET_TX;

    return result;
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
void halBioCom_registerInstruction()
{
    __REGISTER_NEXT_INSTRUCTION = TRUE;
}

/**
 * Adds a new parameter to the transmit buffer.
 *
 * @param parameter The value assigned to the parameter array.
 * @return The index of the new parameter added, or ERROR if something has gone awry.
 */
int halBioCom_addParameter(byte parameter)
{
    int i = 5 + TX_PACKET_LENGTH;    // Get the index of the newest parameter to be added
    if ( i < TRX_BUFFER_SIZE - 1 )   // Check if the buffer allows anymore parameters
    {
        __tx[i] = parameter;           // If so, set the parameter
        return ++TX_PACKET_LENGTH;   // and return the index of such parameter.
    }

    return ERROR;
}

/**
 * Internal method to set the instruction of the transmit buffer.
 *
 * @param inst Instruction to be set.
 */
void halBioCom_setInstruction(byte instruction)
{
    clearBuffer(__tx);
    clearBuffer(__rx);

    if ( __REGISTER_NEXT_INSTRUCTION && instruction == INS_WRITE_DATA )
    {
        TX_PACKET_INSTRUCTION = INS_REG_WRITE;
        __REGISTER_NEXT_INSTRUCTION = FALSE;
    }
    else
    {
        TX_PACKET_INSTRUCTION = instruction;
    }
}

/**
 * Sets USCI A0 to UART mode, and configures required registers
 * to allow communication with AX12 actuators.
 *
 * It uses Subsystem Master Clock (SMCLK) as source.
 *
 * WARNING!
 * -----------------------------------------------------------------
 * This API uses Timer A1 to manage certain state conditions. Do not
 * implement an ISR based on TIMER A1.
 *
 * And it is REQUIRED that TIMER A1 uses SMCLK signal at 16MHz!
 * -----------------------------------------------------------------
 */
void halBioCom_initialize()
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

    __delay(1000); // Wait 1ms due to port line unstability.
}

/**
 * This function allows to reset USCI A0 to factory conditions.
 */
void halBioCom_shutdown()
{
    UCA0CTL1 |= UCSWRST; // Enable Software reset. USCI logic held in reset state

    UCA0IE &= ~UCRXIE; // Disable Receive interruptions

    clearBuffer(__tx);
    clearBuffer(__rx);

    UCA0CTL0 = 0;
    UCA0CTL1 = 1;

    UCA0BR0 = 0;
    UCA0BR1 = 0;

    UCA0MCTL = 0;

    P3SEL &= ~( BIT4 | BIT5 );
    P3REN &= ~( BIT4 | BIT5 );
    P3OUT &= ~( BIT4 | BIT5 );

    P3DIR &= ~BIT7;
    P3SEL &= ~BIT7;
    P3OUT &= ~BIT7;

    __STALL = FALSE;
    __TIMEOUT_STALL = FALSE;
    __BUFFER_OVERFLOW = FALSE;


    UCA0CTL1 &= ~UCSWRST; // Disable Software reset.
}

/**
 * Returns the pointer to the transmit buffer.
 *
 * @returns The pointer to the transmit buffer.
 */
byte* halBioCom_getTX()
{
    return __tx;
}

/**
 * Returns the pointer to the receive buffer.
 *
 * @returns The pointer to the receive buffer.
 */
byte* halBioCom_getRX()
{
    return __rx;
}
