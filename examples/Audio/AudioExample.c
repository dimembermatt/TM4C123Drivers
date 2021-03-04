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


DACConfig config = {
    {PIN_B0, PIN_B1, PIN_B2, PIN_B3, PIN_COUNT, PIN_COUNT}
};

/** Initializes both onboard switches to test triggers. */
int main(void) {
    PLL_Init(Bus80MHz);
    playSound(0, 440, SineWave, config);
	
    // Connect up with a speaker using pins B0 - B3 and try to listen for 440 Hz sine wave.
	while(1) {}
}
