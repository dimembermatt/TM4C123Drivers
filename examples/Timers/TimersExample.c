/**
 * File name: TimersExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example programs to demonstrate the capabilities of normal and
 * wide timers. Includes SysTick.
 * Authors: Matthew Yu.
 * Last Modified: 04/07/21
 * 
 * Modify __MAIN__ on L14 to determine which main method is executed.
 * __MAIN__ = 0 - Initialization and operation of multiple timers at different frequencies.
 *          = 1 - Initialization of a timer, modifying its period, and stopping it.
 * 
 */
#define __MAIN__ 0

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/Misc/Misc.h>
#include <lib/Timers/Timers.h>
#include <lib/GPIO/GPIO.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#if __MAIN__ == 0
volatile uint32_t counter0A = 0;
volatile uint32_t counter0B = 0;
volatile uint32_t counter1A = 0;
volatile uint32_t counter2A = 0;

void dummyTask1(void) { ++counter0A; }

void dummyTask2(void) { ++counter1A; }

void dummyTask3(void) {
    ++counter2A;
    
    if (counter2A == 100) {
        /* Place a breakpoint at L38! Notice that at counter2A = 100, counter0A ~= 25, counter1A ~= 50. */
        TimerUpdatePeriod(TIMER_2A, freqToPeriod(100, MAX_FREQ)); 
    }
    if (counter2A == 105) {
        /* Place a breakpoint at L42! Notice here that when counter2A = 105, counter0A ~= 30, counter1A ~= 60. */
        uint8_t i = 0;
    }
}

void dummyTask4(void) { ++counter0B; }

int main(void) {
    /** 
     * This program demonstrates the initialization and operation of four
     * timers at different frequencies.
     */
    PLL_Init(Bus80MHz);
    DisableInterrupts();

    TimerConfig_t timers[4] = {
        /* The first timer has keyed arguments notated to show you what each positional argument means. */
        {.timerID=TIMER_0A, .period=freqToPeriod(100, MAX_FREQ), .isPeriodic=true, .priority=5, .handlerTask=dummyTask1},
        {         TIMER_0B,         freqToPeriod(800, MAX_FREQ),             true,           5,              dummyTask4},
        {         TIMER_1A,         freqToPeriod(200, MAX_FREQ),             true,           5,              dummyTask2},
        {         TIMER_2A,         freqToPeriod(400, MAX_FREQ),             true,           5,              dummyTask3},
    };

    /* Initialize four timers based on the timer configuration array above. */
    TimerInit(timers[0]);
    TimerInit(timers[1]);
    TimerInit(timers[2]);
    TimerInit(timers[3]);

    EnableInterrupts();
    while (1) {
        /** 
         * View in debugging mode with counter0A, counter0B, counter1A, and
         * counter2A added to watch 1. Run for a while, then check register
         * value ratios. Should be 1 : 0 : 2 : 4.
         * Note that you can't run both A side and B side timers at once.
         * Try it again with isPeriod set to false for single trigger mode,
         * or with different priorities on the timers. What do you see?
         *
         * NOTE: As of (03/13/21) B-side timers do not activate properly. 
         */
        WaitForInterrupt();
    };
}

#elif __MAIN__ == 1
/** This dummy task increments an internal counter and toggles an LED. */
void dummyTask(void) {
    static uint32_t counter = 0;
    ++counter;
    GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));

    if (counter == 25) {
        /* At about 5 seconds, the LED toggle frequency should double. */
        TimerUpdatePeriod(TIMER_0A, freqToPeriod(10, MAX_FREQ)); 
    }

    if (counter == 75) {
        /* At about 10 seconds, the LED toggle should stop. */
        TimerStop(TIMER_0A);
    }
}

int main(void) {
    /**
     * This program demonstrates starting a timer, updating its period, and then
     * stopping the timer.
     */
    PLL_Init(Bus80MHz);
    DisableInterrupts();

    /* Initialize a GPIO LED on PF1. */
    GPIOConfig_t PF1Config = {
        .GPIOPin=PIN_F1, 
        .pull=PULL_DOWN, 
        .isOutput=true, 
        .isAlternative=false, 
        .alternateFunction=0, 
        .isAnalog=false
    };
    GPIOInit(PF1Config);

    /* Initialize a timer executing at 5 Hz. */
    TimerConfig_t timerConfig = {TIMER_0A, freqToPeriod(5, MAX_FREQ), true, 5, dummyTask};
    TimerInit(timerConfig);

    EnableInterrupts();
    while (1) { WaitForInterrupt(); }
}
#endif
