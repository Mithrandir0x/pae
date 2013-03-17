
#include <msp430x54xa.h>

typedef unsigned char byte;

void initialize_ucs()
{
	__bis_SR_register(SCG0); // Toggle the Frequency Locked Loop (Should disable FLL)
							 // More information at page 30 (Operating Modes)

	UCSCTL0 = 0; // DCO = 0, MOD = 0
	UCSCTL1 = DCORSEL_7; // DCORSEL = 7 (111)
			// This means that the FLL reference is XT2CLK (And this is only valid for F543x(A?) models)
			// Although it may also mean that FLL reference is ~16MHz (??)

	UCSCTL2 = 487 + FLLD_1; // f_DCOCLK / 1 (Loop Divider) | FLLN = 487
	UCSCTL3 = 0; // SELREF = 000 (XT1CLK) FLL Reference select | f_FLLREFCLK / 1 FLL Reference divider

	UCSCTL4 = SELA__XT1CLK     // Alternative Clock source comes from XT1 Oscillator
			| SELS__DCOCLKDIV  // Subsystem Master Clock source comes from Digital-Controlled Oscillator (Divided)
			| SELM__DCOCLKDIV; // Master Clock source comes from Digital-Controlled Oscillator (Divided)

	UCSCTL5 = DIVA_1 // Selects ACLK source divider to 1
			| DIVS_1 // Selects SMCLK source divider to 1
			| DIVM_1;// Selects MCLK source divider to 1 (??)

	__bis_SR_register(SCG0);  // Toggle the Frequency Locked Loop (Should enable FLL)

	P11DIR = 0x07; // P11.0 to P11.2 are selected as OUTPUT
	P11SEL = 0x07; // ACLK, MCLK and SMCLK
}

void initialize_uart()
{
	UCA0CTL1 |= UCSWRST; // Enable Software reset. USCI logic held in reset state

	UCA0CTL0 = 0; // Disable parity  // AX-12 Requirement
				  // Set odd parity
				  // LSB First
				  // 8-bit data      // AX-12 Requirement
				  // One stop bit    // AX-12 Requirement
				  // UART Mode
				  // Obviously, asynchronous mode // AX-12 Requirement

	UCA0CTL1 |= UCSSEL__SMCLK; // Set USCI Clock source from SMCLK (Selects the BRCLK also)

	UCA0BRW = 1; // Sets baud rate to 1 (??)

	UCA0MCTL = UCOS16; // Enable oversampling mode, but do not select any kind of modulation

	P3SEL |= 0x30; // Port lines P3.4 and P3.5 selected as default I/O function
		// P3.4 = UART0TX
		// P3.5 = UART0RX

	P3REN |= 0x30; // Enable "Pull-Up/Pull-Down" resistor for P3.4 and P3.5
	P3OUT |= 0x30; // Set initial state as "Pull-Up"

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
}

/**
 * Dynamixel AX-12 Packet Types:
 *  - INSTRUCTION
 *  - STATUS
 *
 * Instruction Packet Spec:
 *
 * 		[0xFF] [0xFF] [ID] [LNG] [INT] [PA_1] [PA_2] ... [PA_N] [CHKSM]
 *
 * Status Packet Spec:
 *
 * 		[0xFF] [0xFF] [ID] [LNG] [ERR] [PA_1] [PA_2] ... [PA_N] [CHKSM]
 */

typedef struct {
	byte error;
	byte instruction;
	byte* parameters;
	byte length;
} AX_12_PACKET;

void main()
{
}
