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
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../inc/GPIO.h"
#include "../lib/DAC/Audio.h"
#include "../lib/DAC/DAC.h"
#include "../inc/TExaS.h"


// measures analog voltage on PD3
void ScopeTask(void){  // called 10k/sec
  UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}

DACConfig config = {
    {PIN_B0, PIN_B1, PIN_B2, PIN_B3, PIN_B4, PIN_B5}
};

/** Initializes both onboard switches to test triggers. */
int main(void) {
    TExaS_SetTask(&ScopeTask);
    PLL_Init(Bus80MHz);
    playSound(0, 440, Test, config);
	EnableInterrupts();

    // Connect up with a speaker using pins B0 - B3 and try to listen for 440 Hz sine wave.
	while(1) {}
}
