
#include <msp430.h> 
#include <hal_common.h>
#include <hal_bio_ax12.h>
#include <hal_buttons.h>
#include <hal_led.h>
#include <hal_timer.h>
#include <hal_ucs.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    _disable_interrupt();

    halUCS_set16MFrequency();

    _enable_interrupt();

    halBioAX12_initialize();

    halLed_sx_initialize();
    halLed_sx_setLed(LED_RED, ON);
    halLed_sx_setLed(LED_YELLOW, OFF);

    halLed_rx_initialize();
    halLed_rx_setLed(LED_RX_ALL, OFF);

    halTimer_b_initialize(TIMER_CLKSRC_ACLK, TIMER_MODE_UP);
    halTimer_b_setCCRTimedInterruption(TIMER_CCR0, 1000);
    halTimer_b_enableInterruptCCR0();

    halJoystick_initialize();
    halJoystick_setInterruptions(JOYSTICK_ALL, ON);

    halButtons_initialize();
    halButtons_setInterruptions(BUTTON_ALL, ON);
	
	return 0;
}

#pragma vector = TIMERB0_VECTOR
__interrupt void update_leds()
{

}
