/**
 * File name: PWMExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the low level PWM driver.
 * Authors: Matthew Yu.
 * Last Modified: 04/06/21
 */
/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Misc/Misc.h>
#include <lib/PWM/PWM.h>


void EnableInterrupts(void);	// Defined in startup.s
void DisableInterrupts(void);	// Defined in startup.s
void WaitForInterrupt(void);	// Defined in startup.s

struct PWMConfig pwmConfigPB5 = {
	.source=DEFAULT,
	.config={M0_PB5, PWMA}
};

int main(void) {
	DisableInterrupts();

	PLL_Init(Bus80MHz);

	/* Initialize a PWM output on PB5 with a frequency of 1Hz at 50% 
	 * duty cycle. */
	PWMInit(pwmConfigPB5, freqToPeriod(1, MAX_FREQ), 50);
	
	EnableInterrupts();
	
	while (1) {
		/* Run an LED at PB5 or hook it up to an O-scope. Check to
		 * see if the output signal is as expected. */
		delayMillisec(1000);
		WaitForInterrupt();
	};
}
