/**
 * @file ADCExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief An example project showing how to use the ADC driver.
 * @version 0.1
 * @date 2022-02-27
 * @copyright Copyright (c) 2021
 * @note
 * Modify __MAIN__ on L13 to determine which main method is executed.
 * __MAIN__ = 0 - Initialization and software sampling of a default ADC and pin.
 *          = 1 - Initialization software sampling of multiple pins on a single ADC.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/ADC/ADC.h>
#include <lib/Timer/Timer.h>


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
    DelayInit();

    /* Initialize a default SW controlled ADC on PE3. The default utilizes
       ADC Module 0, ADC Sample Sequencer SS0, and ADCSequence position ZERO.
       It is also by default the end sample, so the sequencer completes its
       sampling at position zero. */
    ADCConfig_t adcConfig = {};
    ADC_t adc = ADCInit(adcConfig);

    volatile uint16_t adcOutput = 0;
    volatile float adcOutputVoltage = 0;

    EnableInterrupts();
    while (1) {
        /* Every 300ms, sample from PE3. */
        DelayMillisec(300);
        adcOutput = ADCSampleSingle(adc);
        adcOutputVoltage = 3.3 * adcOutput / 4096 * 1000;
        /* Put a breakpoint at L53 and a watch for adcOutput. Tie this to 3.3V,
           or 0V, and so on. Do the values make sense? We can convert our reading
           into a voltage using the following formula:
               measured voltage (mV) = adcOutput / 4096 * 3.3 * 1000;
         */
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
    DelayInit();

    /* Initialize a SW controlled ADC sampling on PE3, PE2. PE2 is in position 2
       and should be sampled second. */
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

    uint16_t adcOutputSeq0[8] = {0};

    EnableInterrupts();
    while (1) {
        /* Every 300ms, sample from PE3. */
        DelayMillisec(100);
        ADCSampleSequencer(
            adcPE3Config.module,
            adcPE3Config.sequencer,
            adcOutputSeq0
        );
        uint8_t i = 0;
        for (; i < 8; ++i)
            adcOutputSeq0[i] = 0;
    };
}
#endif
