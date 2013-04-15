
#include <msp430x54xa.h>
#include <hal_common.h>
#include <hal_led.h>
#include <hal_timer.h>
#include <hal_ucs.h>
#include <hal_bio_ax12.h>

/**
 * Dynamixel AX-12 Packet Types:
 *  - INSTRUCTION
 *  - STATUS
 *
 * Instruction Packet Spec:
 *
 * 		[0xFF] [0xFF] [ID] [LNG] [INST] [PA_1] [PA_2] ... [PA_M] [CHKSM]
 * 		  0      1     2     3     4     5      6          N-1     N
 *      [FF] [FF] [04] [02] [00] [FE]
 *        0    1    2    3    4    5
 *
 * Status Packet Spec:
 *
 * 		[0xFF] [0xFF] [ID] [LNG] [ERR] [PA_1] [PA_2] ... [PA_M] [CHKSM]
 *
 * 		FF FF 02 02 00 ~~
 */

void initialize_super_dooper_ai()
{
    halLed_sx_initialize();
    halLed_sx_setLed(LED_RED, ON);
    halLed_sx_setLed(LED_YELLOW, OFF);

    halTimer_b_initialize(TIMER_CLKSRC_ACLK, TIMER_MODE_UP);
    halTimer_b_setCCRTimedInterruption(TIMER_CCR0, 1000);
    halTimer_b_enableInterruptCCR0();
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer (good dog)

    _disable_interrupt();

    halUCS_set16MFrequency();
    halUCS_setFrequencyDiagnosis(ON);

    halBioAX12_initialize();

    initialize_super_dooper_ai();

    _enable_interrupt();

    halBioAX12_setLed(1, ON);
    halBioAX12_setLed(2, ON);
    halBioAX12_setLed(3, ON);
    halBioAX12_setLed(4, ON);

    while (1);
}

#pragma vector = TIMERB0_VECTOR
__interrupt void update_leds()
{
    halTimer_b_disableInterruptCCR0();

    halLed_sx_toggleLed(LED_SX_ALL);

    //halBioAX12_setLed(2, ON);

    halTimer_b_enableInterruptCCR0();
}
