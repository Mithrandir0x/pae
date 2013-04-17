
#include <msp430x54xa.h>
#include <hal_common.h>
#include <hal_bio_ax12.h>
#include <hal_buttons.h>
#include <hal_led.h>
#include <hal_timer.h>
#include <hal_ucs.h>

volatile int __MOVE_FORWARD = FALSE;
volatile int __MOVE_BACKWARD = FALSE;
volatile int __STOP = FALSE;
volatile int __TURN_LEFT = FALSE;
volatile int __TURN_RIGHT = FALSE;

void initialize_super_dooper_ai()
{
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
}

void main()
{
    int flag = 0;

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer (good dog)

    _disable_interrupt();

    halUCS_set16MFrequency();
    halUCS_setFrequencyDiagnosis(ON);

    _enable_interrupt();

    halBioAX12_initialize();

    initialize_super_dooper_ai();

    flag = halBioAX12_enableEndlessTurn(1);
    halLed_rx_setLed(LED_R1, flag);
    //flag = halBioAX12_setLed(1, ON);
    //halLed_rx_setLed(LED_R2, flag);
    //flag = halBioAX12_setMovingSpeed(1, 0, AX12_CW);
    //halLed_rx_setLed(LED_R3, flag);

    //halBioAX12_enableEndlessTurn(2);
    //halBioAX12_enableEndlessTurn(3);
    //halBioAX12_enableEndlessTurn(4);

    while (1)
    {
        /* if ( __MOVE_FORWARD )
        {
            __MOVE_FORWARD = FALSE;

            halBioAX12_setLed(1, ON);
            halBioAX12_setMovingSpeed(1, 256, AX12_CW);

            halBioAX12_setLed(2, ON);
            halBioAX12_setMovingSpeed(2, 256, AX12_CW);

            halBioAX12_setLed(3, ON);
            halBioAX12_setMovingSpeed(3, 256, AX12_CCW);

            halBioAX12_setLed(4, ON);
            halBioAX12_setMovingSpeed(4, 256, AX12_CCW);
        }

        if ( __MOVE_BACKWARD )
        {
            __MOVE_BACKWARD = FALSE;

            halBioAX12_setLed(1, ON);
            halBioAX12_setMovingSpeed(1, 256, AX12_CCW);

            halBioAX12_setLed(2, ON);
            halBioAX12_setMovingSpeed(2, 256, AX12_CCW);

            halBioAX12_setLed(3, ON);
            halBioAX12_setMovingSpeed(3, 256, AX12_CW);

            halBioAX12_setLed(4, ON);
            halBioAX12_setMovingSpeed(4, 256, AX12_CW);
        }

        if ( __STOP )
        {
            __STOP = FALSE;

            flag = halBioAX12_setLed(1, OFF);
            halLed_rx_setLed(LED_R1, flag);
            halBioAX12_setMovingSpeed(1, 0, AX12_CW);

            halBioAX12_setLed(2, OFF);
            halBioAX12_setMovingSpeed(2, 0, AX12_CW);

            halBioAX12_setLed(3, OFF);
            halBioAX12_setMovingSpeed(3, 0, AX12_CCW);

            halBioAX12_setLed(4, OFF);
            halBioAX12_setMovingSpeed(4, 0, AX12_CCW);
        }

        if (__TURN_LEFT )
        {
            __TURN_LEFT = FALSE;

            halBioAX12_setLed(1, ON);
            halBioAX12_setMovingSpeed(1, 256, AX12_CW);

            halBioAX12_setLed(2, ON);
            halBioAX12_setMovingSpeed(2, 256, AX12_CW);

            halBioAX12_setLed(3, ON);
            halBioAX12_setMovingSpeed(3, 256, AX12_CW);

            halBioAX12_setLed(4, ON);
            halBioAX12_setMovingSpeed(4, 256, AX12_CW);

        }

        if (__TURN_RIGHT )
        {
            __TURN_RIGHT = FALSE;

            halBioAX12_setLed(1, ON);
            halBioAX12_setMovingSpeed(1, 256, AX12_CCW);

            halBioAX12_setLed(2, ON);
            halBioAX12_setMovingSpeed(2, 256, AX12_CCW);

            halBioAX12_setLed(3, ON);
            halBioAX12_setMovingSpeed(3, 256, AX12_CCW);

            halBioAX12_setLed(4, ON);
            halBioAX12_setMovingSpeed(4, 256, AX12_CCW);

        } */
    }
}

#pragma vector = PORT2_VECTOR
__interrupt void on_button_interruption(void)
{
    halButtons_setInterruptions(BUTTON_ALL, OFF);
    halJoystick_setInterruptions(JOYSTICK_ALL, OFF);

    switch ( P2IFG )
    {
        case JOYSTICK_RIGHT:
            __TURN_RIGHT = TRUE;
            break;
        case JOYSTICK_LEFT:
            __TURN_LEFT = TRUE;
            break;
        case JOYSTICK_UP:
            __MOVE_FORWARD = TRUE;
            break;
        case JOYSTICK_DOWN:
            __MOVE_BACKWARD = TRUE;
            break;
        case JOYSTICK_CENTER:
            break;
        case BUTTON_S1:
            __STOP = TRUE;
            break;
        case BUTTON_S2:
            __MOVE_FORWARD = TRUE;
            break;
    }

    P2IFG = 0;

    halButtons_setInterruptions(BUTTON_ALL, ON);
    halJoystick_setInterruptions(JOYSTICK_ALL, ON);
}

#pragma vector = TIMERB0_VECTOR
__interrupt void update_leds()
{
    halTimer_b_disableInterruptCCR0();

    halLed_sx_toggleLed(LED_SX_ALL);

    halTimer_b_enableInterruptCCR0();
}
