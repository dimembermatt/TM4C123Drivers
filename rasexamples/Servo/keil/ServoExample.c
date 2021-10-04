/**
 * @file ServoExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Making a servo move, change direction, and adjust its speed.
 * @version 0.1
 * @date 2021-09-29
 * @copyright Copyright (c) 2021
 */

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/Timer/Timer.h>
#include <raslib/Servo/Servo.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

int main(void) {
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    DelayInit();

    /* Red onboard LED. */
    GPIOConfig_t PF1 = {
        .pin=PIN_F1,
        .isOutput=true
    };
    GPIOInit(PF1);
    /* Blue onboard LED. */
    GPIOConfig_t PF2 = {
        .pin=PIN_F2,
        .isOutput=true
    };
    GPIOInit(PF2);

    /* Servo configuration. */
    ServoConfig_t config = {
        .pin=PIN_B6,
        .timerID=TIMER_0A
    };
    PWM_t servo = ServoInit(config);

    EnableInterrupts();
    while(1) {
        /* Make the servo stall for 5 seconds. This should be not moving.
           To tune your servo, you want to turn the front screw until the servo
           is not moving when the light is off, and to move when the light is
           on. */
        GPIOSetBit(PIN_F1, 0);
        GPIOSetBit(PIN_F2, 0);
    	ServoSetSpeed(servo, 0);
        DelayMillisec(5000);

        /* Make the servo go backward for 5 seconds. */
        GPIOSetBit(PIN_F1, 1);
        GPIOSetBit(PIN_F2, 0);
        ServoSetSpeed(servo, -100);
        DelayMillisec(2500);
        ServoSetSpeed(servo, -20);
        DelayMillisec(2500);

        /* Make the servo stall for 5 seconds. This should be not moving. */
        GPIOSetBit(PIN_F1, 0);
        GPIOSetBit(PIN_F2, 0);
        ServoSetSpeed(servo, 0);
        DelayMillisec(5000);

        /* Make the servo go forward for 5 seconds. */
        GPIOSetBit(PIN_F1, 0);
        GPIOSetBit(PIN_F2, 1);
        ServoSetSpeed(servo, 20);
        DelayMillisec(2500);
        ServoSetSpeed(servo, 100);
        DelayMillisec(2500);

        /* Play around with the servo speed values. Note from Servo.h that
           the bounds are [-100, 100]. Are there some speeds where it's not
           moving as expected, even with tuning? Plot this out and make a graph
           of expected speed vs actual speed! Is the graph linear or nonlinear!
           Your observations will be useful in controlling your robot to move in
           straight lines in exact distances. */
    }
}
