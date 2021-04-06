/**
 * File name: GPIOExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the low level GPIO driver.
 * Authors: Matthew Yu.
 * Last Modified: 04/05/21
 */
/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Misc/Misc.h>


GPIOConfig_t PF1Config = {
	.GPIOPin=PIN_F1, 
	.pull=PULL_DOWN, 
	.isOutput=true, 
	.isAlternative=false, 
	.alternateFunction=0, 
	.isAnalog=false
};

GPIOConfig_t PF2Config = {
	PIN_F2, 
	PULL_DOWN, 
	true, 
	false, 
	0, 
	false
};

int main(void) {
	PLL_Init(Bus80MHz);

	GPIOInit(PF1Config);
	GPIOInit(PF2Config);

	GPIOSetBit(PIN_F1, 1);
	GPIOSetBit(PIN_F2, 0);
	while (1) {
		delayMillisec(1000);
		
		/* See when running that a blue and red light flash alternately. */
		GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));
		GPIOSetBit(PIN_F2, !GPIOGetBit(PIN_F2));
	};
}
