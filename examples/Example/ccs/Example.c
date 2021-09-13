/**
 * File name: Example.h
 * Devices: LM4F120; TM4C123
 * Description: <PROGRAM DESCRIPTION>
 * Authors: <YOUR NAME>
 * Last Modified: <TODAY'S DATE>
 */

/** General imports. These can be declared the like the example below. */
#include <stdlib.h>

/** 
 * Device specific imports. Include files and library files are accessed like
 * the below examples.
 */
#include <inc/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timers/Timers.h>


/** 
 * These function declarations are defined in the startup.s assembly file for
 * managing interrupts. 
 */
void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

/** 
 * Put your global variables and function declarations and/or implementations
 * here. 
 */
void heartbeat(void) { GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1)); }

/** Your main execution loop. */
int main(void) {
    /** Hardware and software initializations. */

    /* Clock setup. */
    PLL_Init(Bus80MHz);
    DisableInterrupts();

    /* Initialize PF1 GPIO (red LED) to flash at 1 Hz. */
    GPIOConfig_t PF1Config = {
        .GPIOPin=PIN_F1,
        .pull=PULL_DOWN,
        .isOutput=true,
        .isAlternative=false,
        .alternateFunction=0,
        .isAnalog=false
    };
    GPIOInit(PF1Config);

    TimerConfig_t heartbeatTimerConfig = {
        .timerID=TIMER_0A,
        .period=freqToPeriod(2, MAX_FREQ),
        .isPeriodic=true,
        .priority=6,
        .handlerTask=heartbeat
    };
    TimerInit(heartbeatTimerConfig);

    EnableInterrupts();
    
    /** Main loop. Put your program here! */
    while (1) {
        WaitForInterrupt();
    }
}
