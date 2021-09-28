/**
 * @file RGBExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief TM4C goes wild with RGB lighting.
 * @version 0.1
 * @date 2021-09-27
 * @copyright Copyright (c) 2021
 * @note Warning to users who have epilepsy - bright flashing colors.
 */

/** General imports. These can be declared the like the example below. */
#include <stdlib.h>

/** 
 * Device specific imports. Include files and library files are accessed like
 * the below examples.
 */
#include <lib/PLL/PLL.h>
#include <raslib/RGB/RGB.h>

/** 
 * These function declarations are defined in the startup.s assembly file for
 * managing interrupts. 
 */
void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

int main(void) {
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Warning to users who have epilepsy - bright flashing colors. */
    RGBInit();

    EnableInterrupts();

    while(1) {
        WaitForInterrupt();
    }
}
