/**
 * File name: PWMExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the low level PWM driver.
 * Authors: Matthew Yu.
 * Last Modified: 04/17/21
 * 
 * Modify __MAIN__ on L12 to determine which main method is executed.
 * __MAIN__ = 0 - Initialization and management of a timer acting as a PWM for low freq.
 *          = 1 - Initialization and management of a PWM module for high freq.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Misc/Misc.h>
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
    PLL_Init(Bus80MHz);
    DisableInterrupts();

    /* Initialize SysTick for delay calls.*/
    delayInit();
    
    PWMConfig_t pwmConfigPF1 = {
        .source=TIMER,
        .config={
			.timerSelect={
				.pin=PIN_F1,
				.timerID=TIMER_0A
			}
		}
    };

    EnableInterrupts();
    uint8_t mode = 0;
    while(1) {
        switch (mode) {
            case 0:
                /* The LED connected to PF1 should flash at 2 Hz with even on-off times. */
                PWMInit(pwmConfigPF1, freqToPeriod(2, MAX_FREQ), 50);
                break;
            case 1:
                /* The LED connected to PF1 should flash at 2 Hz with short on and long off times. */
                PWMUpdateConfig(pwmConfigPF1, freqToPeriod(2, MAX_FREQ), 12);
                break;
            case 2:
                /* The LED connected to PF1 should flash at 5 Hz with even on-off times. */
                PWMUpdateConfig(pwmConfigPF1, freqToPeriod(5, MAX_FREQ), 50);
                break;
            case 3:
                /* The LED connected to PF1 should stop in the on position. */
                PWMStop(pwmConfigPF1);
                GPIOSetBit(PIN_F1, 1);
                break;
        }
        delayMillisec(2000);
        mode = (mode + 1) % 4;
    }
}

#elif __MAIN__ == 1
int main(void) {
    /**
     * This program demonstrates initializing initializing the TM4C PWM module,
     * and progressively changing the brightness using the duty cycle.
     */
    PLL_Init(Bus80MHz);
    DisableInterrupts();

    /* Initialize SysTick for delay calls.*/
    delayInit();
    
    PWMConfig_t pwmConfigPF1 = {
        .source=DEFAULT,
        .config={
			.pwmPin=M1_PF1
		}
    };
    EnableInterrupts();
	PWMInit(pwmConfigPF1, freqToPeriod(2000, MAX_FREQ), 0);
	uint8_t dutyCycle = 0;
    while(1) {
        delayMillisec(100);
		/**
		 * Run this on an oscilloscope to check if the waveform matches 2 kHz.
		 * What if you change the frequency? Duty cycle?
		 * At what minimum frequency/period can you load onto the pin and still
		 * have an accurate waveform?
		 */
		PWMUpdateConfig(pwmConfigPF1, freqToPeriod(2000, MAX_FREQ), dutyCycle);
		dutyCycle = (dutyCycle + 1)%100;
    }
}
#endif
