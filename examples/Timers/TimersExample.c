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

/** Initializes both onboard switches to test triggers. */
int main(void) {
    PLL_Init(Bus80MHz);
    DisableInterrupts();

    TimerInit(TIMER_0A, freqToPeriod(100, MAX_FREQ), dummyTask);
    TimerInit(TIMER_1A, freqToPeriod(200, MAX_FREQ), dummyTask2);
    TimerInit(TIMER_2A, freqToPeriod(400, MAX_FREQ), dummyTask3);

    EnableInterrupts();

    while (1) {
        // View in debugging mode with counter0A, counter1A, and counter2A added
        // to watch 1. Run for a while, then check register value ratios. Should
        // be 1 : 2 : 4.
        WaitForInterrupt();
    };
}
