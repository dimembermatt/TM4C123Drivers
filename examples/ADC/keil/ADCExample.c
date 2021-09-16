/**
 * File name: ADExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the capabilities of the onboard ADC.
 * Authors: Matthew Yu.
 * Last Modified: 09/15/21
 * 
 * Modify __MAIN__ on L12 to determine which main method is executed.
 * __MAIN__ = 0 - Initialization and software sampling of a default ADC and pin.
 *          = 1 - Initialization software sampling of multiple pins on a single ADC.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/ADC/ADC.h>
#include <lib/Misc/Misc.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#if __MAIN__ == 0
int main(void) {
    /**
     * This program demonstrates initializing the pin PE3 as an ADC and reading
     * from it at a fixed rate in software.
     */
    PLL_Init(Bus80MHz);
    DisableInterrupts();

    /* Initialize SysTick for delay calls.*/
    delayInit();
    
    /* Initialize a default SW controlled ADC on PE3. The default utilizes
     * ADC Module 0, ADC Sample Sequencer SS0, and ADCSequence position ZERO.
     * It is also by default the end sample, so the sequencer completes its
     * sampling at position zero.
     */
    ADCConfig_t adcConfig = {};
    ADC_t adc = ADCInit(adcConfig);

    volatile uint32_t adcOutput = 0;

    EnableInterrupts();
    while (1) {
        /* Every 300ms, sample from PE3. */
        delayMillisec(300);
        adcOutput = ADCSample(adc);
    };
}
#elif __MAIN__ == 1

#endif
