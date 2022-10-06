/**
 * @file SSIExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief An example project showing how to use the SSI driver for SPI communication.
 * @version 0.1
 * @date 2021-09-23
 * @copyright Copyright (c) 2021
 */

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/SSI/SSI.h>
#include <lib/Timer/Timer.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#define BUFFER_SIZE 16
uint16_t receiveBuffer[BUFFER_SIZE] = { 0 };
void sendSSI(uint32_t * args) {
    static uint8_t i = 0;
    static uint16_t val = 0x01;
    SPIWrite(SSI2_PB, val);
    receiveBuffer[i] = SPIRead(SSI2_PB);
    if (i == BUFFER_SIZE-1) {
        uint8_t stop = 0;
    }
    i = (i + 1) % BUFFER_SIZE;
    val = ((val << 1) & 0xFFFF);
}

Timer_t timer;

int main(void) {
    /** 
     * This program demonstrates the initialization and operation of the SSI
     * unit operating as an SPI bus. It writes an incrementing counter to the PB
     * lines.
     * 
     * This is set to run at 10 MHz and is in loopback mode so no external device
     * is needed.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    SSIConfig_t ssiConfig = {
        .ssi=SSI2_PB, 
        .dataSize=SSI_SIZE_16,
        .ssiPrescaler=8,
        .ssiClockModifier=0,
        .isReceiving=false,
        .frameFormat=FREESCALE_SPI,
        .isSecondary=false,
        .isClockLow=false,
        .polarity=SSI_FIRST_EDGE,
        .isLoopback=true /* Set this to false for your own device! */
    };

    /* Initialize an SPI bus operating on PB. See SSI.h on pins used. */
    SSIInit(ssiConfig);

    TimerConfig_t timerConfig = {
        .timerID=SYSTICK, 
        .period=freqToPeriod(1000, MAX_FREQ), 
        .timerTask=sendSSI,
        .isPeriodic=true, 
        .priority=2, 
        .timerArgs=NULL
    };
    
    /* Initialize a timer sending bits across SPI regularly. */
    timer = TimerInit(timerConfig);
		TimerStart(timer);

    EnableInterrupts();

    while (1) {
        /* View with an actual oscilloscope the waveforms on the SSICLK, SSITX,
           and SSIFss pins. You should be able to identify the clock activations
           and read the binary value transmitted. 
           
           Additionally, since this is in loopback mode, we should be able to
           view the read buffer in internal memory with the data we want. Put a 
           breakpoint at L31 and run the program until it stops. Check the 
           receiveBuffer in watch; what do you see? */
        WaitForInterrupt();
    };
}
