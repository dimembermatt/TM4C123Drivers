/**
 * File name: GPIOExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the low level GPIO driver.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 */

/** Device specific imports. */
#include <TM4C123Drivers/inc/tm4c123gh6pm.h>
#include <TM4C123Drivers/inc/PLL.h>
#include <TM4C123Drivers/lib/GPIO/GPIO.h>
#include <TM4C123Drivers/lib/Misc/Misc.h>

void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

GPIOConfig_t PF1Config = {PIN_F1, PULL_DOWN, true, false, 0, false};
GPIOConfig_t PF2Config = {PIN_F2, PULL_DOWN, true, false, 0, false};

int main(void) {
    PLL_Init(Bus80MHz);

    GPIOInit(PF1Config);
    GPIOInit(PF2Config);

    GPIOSetBit(PIN_F1, 1);
    GPIOSetBit(PIN_F2, 0);
    while (1) {
        delayMillisec(100);
        // See when running that a blue and red light flash alternately.
        GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));
        GPIOSetBit(PIN_F2, !GPIOGetBit(PIN_F2));
    };
}
