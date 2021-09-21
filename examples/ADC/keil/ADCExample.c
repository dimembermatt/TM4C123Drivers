/**
 * File name: ADExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the capabilities of the onboard ADC.
 * Authors: Matthew Yu.
 * Last Modified: 09/16/21
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
    PLLInit(BUS_80_MHZ);
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
        adcOutput = ADCSampleSingle(adc);
    };
}
#elif __MAIN__ == 1
int main(void) {
    /**
     * This program demonstrates initializing the pin PE3 and PE2 as an ADC and 
     * reading from it at a fixed rate in software.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Initialize SysTick for delay calls.*/
    delayInit();
    
    /* Initialize a SW controlled ADC sampling on PE3, PE2. PE2 is in position 2
	 * and should be sampled second. */
    ADCConfig_t adcPE3Config = {
		.pin=AIN0,
		.position=ADC_SEQPOS_0,
		.isNotEndSample=true
	};
	ADCConfig_t adcPE2Config = {
		.pin=AIN1,
		.position=ADC_SEQPOS_1,
	};
    ADC_t adcPE3 = ADCInit(adcPE3Config);
    ADC_t adcPE2 = ADCInit(adcPE2Config);

    uint32_t adcOutputSeq0[8] = {0};

    EnableInterrupts();
    while (1) {
        /* Every 300ms, sample from PE3. */
        delayMillisec(100);
        ADCSampleSequencer(
			adcPE3Config.module,
			adcPE3Config.sequencer,
			adcOutputSeq0
		);
		for (uint8_t i = 0; i < 8; ++i)
			adcOutputSeq0[i] = 0;
    };
}
#endif
