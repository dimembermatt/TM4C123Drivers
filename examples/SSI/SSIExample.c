/**
 * File name: SSIExample.h 
 * Devices: LM4F120; TM4C123 
 * Description: Example program to demonstrate the SSI SPI capability.
 * Authors: Matthew Yu. 
 * Last Modified: 03/13/21
 */

/** Device specific imports. */
#include <TM4C123Drivers/inc/PLL.h>
#include <TM4C123Drivers/lib/SSI/SSI.h>
#include <TM4C123Drivers/lib/Timers/Timers.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

void sendSSI(void) {
    static uint16_t val = 0x6000;
    SPIWrite(SSI2_PB, val);
	val = ((val + 1) & 0x0FFF) | 0x6000;
}

int main(void) {
    PLL_Init(Bus80MHz);
    DisableInterrupts();

    /* Keyed arguments are notated to show what each positional argument means. */
    SSIConfig_t config = {
        .SSI=SSI2_PB, 
        .frameFormat=FREESCALE_SPI, 
        .isPrimary=true, 
        .dataBitSize=16};
    SSIInit(config);

	TimerConfig_t timer = {
        .timerID=TIMER_0A, 
        .period=freqToPeriod(1000000, MAX_FREQ), 
        .isPeriodic=true, 
        .priority=5, 
        .handlerTask=sendSSI};
    TimerInit(timer);

    EnableInterrupts();

    while (1) {
        /* View with an actual oscilloscope the waveforms on the SSICLK, SSITX,
         * and SSIFss pins. You should be able to identify the clock activations
         * and read the binary value transmitted. */
        WaitForInterrupt();
    };
}
