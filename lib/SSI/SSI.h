/**
 * SSI.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for SSI communication.
 * Authors: Matthew Yu.
 * Author: Matthew Yu
 * Last Modified: 04/17/21
 */
#pragma once

/** General imports. */
#include <stdint.h>
#include <stdbool.h>


typedef enum SSISelect{     /** Clk, Fss, Rx,  Tx.  */
    SSI0_PA=0,      /** PA2, PA3, PA4, PA5. */
    SSI1_PF=1,      /** PF2, PF3, PF0, PF1. */
    SSI2_PB=2,      /** PB4, PB5, PB6, PB7. */
    SSI3_PD=3,      /** PD0, PD1, PD2, PD3. */
    UNDEFINED=4,    /** Invalid SSI Config. */
    SSI1_PD=5,      /** PD0, PD1, PD2, PD3. */
} SSISelect_t;

typedef struct SSIConfig {
    /** Which SSI module and port. */
    SSISelect_t SSI;

    /** Determines the specific frame format of the SSI. */
    enum SSIFrameFormat { FREESCALE_SPI, TI_SYNCHRONOUS_SERIAL, MICROWIRE, RESERVED } frameFormat;

    /** Determines whether the SSI module is Primary or Secondary. */
    bool isPrimary;

    /** Determines whether the SSI is either transmitting or receiving info. */
    bool isTransmitting;

    /** Determines whether the clock line is raised high when not executing. */
    bool isClockDefaultHigh;

    /** 
     * Determines whether the data bits are captured on the 1st or 2nd edge. 
     * True is 2nd edge, False is 1st edge.
     */ 
    bool polarity;

    /** The size of the SSI. From 4 (0x3) to 16 (0xF) bits. */
    uint8_t dataBitSize;
} SSIConfig_t;

/**
 * SSIInit initializes a given SSI module. 
 * @param SSIConfig Configuration details of a given SSI module.
 * @note Potentially add the following parameters:
 *          - Set whether SSI loopback mode is enabled (p. 972)
 *          - Add parameter in SSIConfig_t for bit rate and SysClk speed.
 *          Determine appropriate prescaler and bit rate selection in SSICR0.
 */
void SSIInit(SSIConfig_t SSIConfig);

/**
 * SPIRead attempts to read the latest set of data in the internal buffer. Busy
 * waits until the receive buffer is not empty.
 * @param ssi SSI to check buffer for.
 * @return Right justified 16-bit data in the SSI data register.
 */
uint16_t SPIRead(SSISelect_t ssi);

/**
 * SPIWrite attempts to write data in the internal buffer. Busy
 * waits until the transmit buffer is not full.
 * @param ssi SSI to write into.
 * @param data Right justified 16-bit data to write.
 */
void SPIWrite(SSISelect_t ssi, uint16_t data);
