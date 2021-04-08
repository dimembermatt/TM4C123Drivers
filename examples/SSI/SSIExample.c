/**
 * File name: SSIExample.h 
 * Devices: LM4F120; TM4C123 
 * Description: Example program to demonstrate the SSI SPI capability.
 * Authors: Matthew Yu. 
 * Last Modified: 03/13/21
 */

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/SSI/SSI.h>
#include <lib/Timers/Timers.h>


void EnableInterrupts(void);	// Defined in startup.s
void DisableInterrupts(void);	// Defined in startup.s
void WaitForInterrupt(void);	// Defined in startup.s

void sendSSI(void) {
	static uint16_t val = 0x6004;
	SPIWrite(SSI2_PB, val);
	val = ((val + 1) & 0x0FFF) | 0x6000;
}

int main(void) {
	/** 
	 * This program demonstrates the initialization and operation of the SSI
	 * unit operating as an SPI bus. It writes an incrementing counter to the PB
	 * lines.
	 */
	PLL_Init(Bus80MHz);
	DisableInterrupts();

	SSIConfig_t config = {
		.SSI=SSI2_PB, 
		.frameFormat=FREESCALE_SPI, 
		.isPrimary=true, 
		.isTransmitting=true,
        .isClockDefaultHigh=true,
        .polarity=false,
		.dataBitSize=16};

	/* Initialize an SPI bus operating on PB. See SSI.h on pins used. */
	SSIInit(config);

	TimerConfig_t timer = {
		.timerID=SYSTICK, 
		.period=freqToPeriod(1000, MAX_FREQ), 
		.isPeriodic=true, 
		.priority=2, 
		.handlerTask=sendSSI};
	
	/* Initialize a timer sending bits across SPI regularly. */
	TimerInit(timer);

	EnableInterrupts();

	while (1) {
		/* View with an actual oscilloscope the waveforms on the SSICLK, SSITX,
		 * and SSIFss pins. You should be able to identify the clock activations
		 * and read the binary value transmitted. */
		WaitForInterrupt();
	};
}
