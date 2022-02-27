/**
 * @file PWMExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief An example project showing how to use the PWM driver.
 * @version 0.1
 * @date 2022-02-27
 * @copyright Copyright (c) 2021
 * @note
 * __FAST__. If you check the target options, in the C/C++ Misc Options, `-D__FAST__` is defined.
 * __FAST__ is present in GPIO.c; it selects between two definitions of GPIOSetBit and GPIOGetBit.
 * __FAST__ trades speed for space complexity. It's very useful for when you
 * want to do GPIO bit handling or very fast consecutive GPIO bit handling!
 * 
 * Modify __MAIN__ on L18 to determine which main method is executed.
 * __MAIN__ = 0 - Initialization and management of a timer acting as a PWM for low freq.
 *          = 1 - Initialization and management of a PWM module for high freq.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timer/Timer.h>
#include <lib/PWM/PWM.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#if __MAIN__ == 0
int main(void) {
    /**
     * This program demonstrates initializing initializing the TM4C PWM module,
     * flashing an LED with it, updating its frequency and duty cycle, and then
     * stopping it.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Initialize SysTick for delay calls.*/
    DelayInit();
    
    PWMConfig_t pwmConfigPF1 = {
        .source=PWM_SOURCE_TIMER,
        .sourceInfo={
            .timerSelect={
                .pin=PIN_F1,
                .timerID=TIMER_0A,
                .period=freqToPeriod(2, MAX_FREQ),
                .isIndividual=false,
                .prescale=0
            }
        },
        .dutyCycle=50
    };

    /* The LED connected to PF1 should flash at 2 Hz with even on-off times. */
    PWM_t pwm = PWMInit(pwmConfigPF1);
    PWMStart(pwm);

    EnableInterrupts();
    
    uint8_t mode = 0;
    while (1) {
        DelayMillisec(5000);
        switch (mode) {
            case 0:
                /* The LED connected to PF1 should flash at 2 Hz with short on and long off times. */
                pwmConfigPF1.sourceInfo.timerSelect.period = freqToPeriod(2, MAX_FREQ);
                pwmConfigPF1.dutyCycle = 12;
                PWMInit(pwmConfigPF1);
                PWMStart(pwm);
                break;
            case 1:
                /* The LED connected to PF1 should flash at 5 Hz with even on-off times. */
                pwmConfigPF1.sourceInfo.timerSelect.period = freqToPeriod(5, MAX_FREQ);
                pwmConfigPF1.dutyCycle = 50;
                PWMInit(pwmConfigPF1);
                PWMStart(pwm);
                break;
            case 2:
                /* The LED connected to PF1 should stop in the on position. */
                PWMStop(pwm);
                GPIOSetBit(PIN_F1, 1);
                break;
        }
        mode = (mode + 1) % 3;
    }
}

#elif __MAIN__ == 1
int main(void) {
    /**
     * This program demonstrates initializing initializing the TM4C PWM module,
     * and progressively changing the brightness using the duty cycle.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Initialize SysTick for delay calls.*/
    DelayInit();
    
    PWMConfig_t pwmConfigPF2 = {
        .source=PWM_SOURCE_DEFAULT,
        .sourceInfo={
            .pwmSelect={
                .pin=M1_PF2,
                .period=freqToPeriod(2000, MAX_FREQ),
                .divisor=PWM_DIV_OFF
            }
        },
        .dutyCycle=1
    };
    
    PWM_t pwm = PWMInit(pwmConfigPF2);
    PWMStart(pwm);

    EnableInterrupts();
    uint8_t dutyCycle = 0;
    while(1) {
        DelayMillisec(50);
        /* Run this on an oscilloscope to check if the waveform matches 2 kHz.
         * What if you change the frequency? Duty cycle?
         * 
         * At what minimum frequency/period can you load onto the pin and still
         * have an accurate waveform? Use the divisor field to try to lower the
         * frequency! */
        pwmConfigPF2.sourceInfo.pwmSelect.period = freqToPeriod(2000, MAX_FREQ);
        pwmConfigPF2.dutyCycle = dutyCycle;
        PWMStop(pwm);
        pwm = PWMInit(pwmConfigPF2);
        PWMStart(pwm);
        dutyCycle = (dutyCycle + 1)%100;
    }
}
#endif
