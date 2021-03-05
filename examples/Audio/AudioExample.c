/**
 * File name: AudioExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate outputting audio from a speaker.
 * Authors: Matthew Yu.
 * Last Modified: 03/03/21
 */

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include <TM4C123Drivers/inc/tm4c123gh6pm.h>
#include <TM4C123Drivers/inc/PLL.h>
#include <TM4C123Drivers/inc/GPIO.h>
#include <TM4C123Drivers/lib/DAC/Audio.h>
#include <TM4C123Drivers/inc/TExaS.h> // For testing.


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

// measures analog voltage on PD3
void ScopeTask(void){  // called 10k/sec
    UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}

DACConfig config = {
    {PIN_B0, PIN_COUNT, PIN_COUNT, PIN_COUNT, PIN_COUNT, PIN_COUNT},
    1
};

/** Initializes both onboard switches to test triggers. */
int main(void) {
    TExaS_SetTask(&ScopeTask);
    PLL_Init(Bus80MHz);
    DisableInterrupts();
    playSound(0, 440*2, Test, config);

	EnableInterrupts();

    // Connect up with a speaker using pins B0 - B3 and try to listen for 440 Hz sine wave.
	while (1) {
        WaitForInterrupt();
    }
}
