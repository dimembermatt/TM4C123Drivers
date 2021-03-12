/**
 * File name: SoundExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate outputting sound waves from a
 * speaker. Example 2 tests the SPI DAC drivers.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 */

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include <TM4C123Drivers/inc/PLL.h>
#include <TM4C123Drivers/lib/DAC/Sound.h>
#include <TM4C123Drivers/lib/Miscellaneous/Misc.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

uint16_t Test[32] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
	4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
};
	
GPIOConfig_t PF1Config = {PIN_F1, PULL_DOWN, true, false, 0, false};

void flipPF1(void) {
    GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));
}

int main(void) {
    union SoundConfig config = {
        //.ssi={SSI2_PB, FREESCALE_SPI, true, 16},
        .pins={PIN_A5, PIN_A4, PIN_A3, PIN_A2, PIN_B6, PIN_COUNT}
    };

    PLL_Init(Bus80MHz);
	DisableInterrupts();

    /* Flash PF1 once per second. */
    GPIOInit(PF1Config);
	TimerInit(TIMER_0A, freqToPeriod(2, MAX_FREQ), flipPF1);

	initializeSoundPlayer(R_DAC);
	EnableInterrupts();

    /* Connect up with a speaker using a R_2R or Binary Weighted DAC
     * in ascending order of pins defined in config.
     * Listen for increasing pitch wave from 1 - 10kHz frequency. */
	uint32_t freq = 1;
	while (1) {
        delayMillisec(100);
    	playSound(0, freq, Test, config);
        freq = (freq + 1) % 10000;
    }
}

int mainSPI(void) {
    union SoundConfig config = {
        .ssi={SSI2_PB, FREESCALE_SPI, true, 16},
    };

    PLL_Init(Bus80MHz);
	DisableInterrupts();

    /* Flash PF1 once per second. */
    GPIOInit(PF1Config);
	TimerInit(TIMER_0A, freqToPeriod(2, MAX_FREQ), flipPF1);

	initializeSoundPlayer(SPI_DAC);
	EnableInterrupts();

    /* Connect up with a speaker using TLV5616 SPI DAC. Use SSI2 pins 
     * defined in SSI.h.
     * Listen for increasing pitch wave from 1 - 10kHz frequency. */
	uint32_t freq = 1;
	while (1) {
        delayMillisec(100);
    	playSound(0, freq, Test, config);
        freq = (freq + 1) % 10000;
    }
}
