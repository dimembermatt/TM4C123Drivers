/**
 * File name: TimersExample.h 
 * Devices: LM4F120; TM4C123 
 * Description: Example program to demonstrate the capabilities of normal and
 * wide timers. Does not include SysTick.
 * Authors: Matthew Yu. 
 * Last Modified: 03/03/21
 */

/** Device specific imports. */
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../lib/Timers/Timers.h"


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

volatile uint32_t counter0A = 0;
volatile uint32_t counter0B = 0;
volatile uint32_t counter1A = 0;
volatile uint32_t counter2A = 0;

void dummyTask(void) {
    ++counter0A;
}

void dummyTask2(void) {
    ++counter1A;
}

void dummyTask3(void) {
	++counter2A;
}

void dummyTask4(void) {
    ++counter0B;
}

/** Initializes both onboard switches to test triggers. */
int main(void) {
    PLL_Init(Bus80MHz);
    DisableInterrupts();

    TimerInit(TIMER_0A, freqToPeriod(100, MAX_FREQ), dummyTask);
    TimerInit(TIMER_0B, freqToPeriod(800, MAX_FREQ), dummyTask4);
    TimerInit(TIMER_1A, freqToPeriod(200, MAX_FREQ), dummyTask2);
    TimerInit(TIMER_2A, freqToPeriod(400, MAX_FREQ), dummyTask3);

    EnableInterrupts();

    while (1) {
        /* View in debugging mode with counter0A, counter0B, counter1A, and
         * counter2A added to watch 1. Run for a while, then check register
         * value ratios. Should be 1 : 0 : 2 : 4. Note that you can't run 
         * both A side and B side timers at once. 
         * NOTE: As of (03/06/21) B-side timers do not activate properly. */
        WaitForInterrupt();
    };
}
