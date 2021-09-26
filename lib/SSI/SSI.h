/**
 * @file SSI.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief SSI (SPI) peripheral driver.
 * @version 0.1
 * @date 2021-09-23
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support DMA control. This driver
 * does not support interrupts.
 */

#pragma once

/** General imports. */
#include <stdint.h>
#include <stdbool.h>

/** @brief SSIModule_t is an enumeration defining a set of pins used for SPI
 *         communication. */ 
typedef enum SSIModule {    /** Clk, Fss, Rx,  Tx.  */
    SSI0_PA=0,              /** PA2, PA3, PA4, PA5. */
    SSI1_PF=1,              /** PF2, PF3, PF0, PF1. */
    SSI2_PB=2,              /** PB4, PB5, PB6, PB7. */
    SSI3_PD=3,              /** PD0, PD1, PD2, PD3. */
    SSI1_PD=5,              /** PD0, PD1, PD2, PD3. */
} SSIModule_t;

/** @brief SSIDataSize is an enumeration specifying the size of the SSI register. */
enum SSIDataSize { 
    SSI_SIZE_4=0x3, 
    SSI_SIZE_5=0x4, 
    SSI_SIZE_6=0x5, 
    SSI_SIZE_7=0x6, 
    SSI_SIZE_8=0x7,
    SSI_SIZE_9=0x8,
    SSI_SIZE_10=0x9,
    SSI_SIZE_11=0xA,
    SSI_SIZE_12=0xB,
    SSI_SIZE_13=0xC,
    SSI_SIZE_14=0xD,
    SSI_SIZE_15=0xE,
    SSI_SIZE_16=0xF
};

/** @brief SSIFrameFormat is an enumeration specifying the SSI protocol. */
enum SSIFrameFormat { 
    FREESCALE_SPI, 
    TI_SYNCHRONOUS_SERIAL, 
    MICROWIRE 
};

/** @brief SSIConfig_t is a user defined struct that specifies a SSI
 *         configuration. */
typedef struct SSIConfig {
    /** 
     * @brief Which SSI module (and port) to use for the communication lines.
     * 
     * Default is SSI0_PA (Port A).
     */
    SSIModule_t ssi;

    /** 
     * @brief The size of the SSI. From 4 to 16 bits wide.
     * 
     * Default is SSI_SIZE_4, or 4 bits.
     */
    enum SSIDataSize dataSize;

    /**
     * @brief Serial clock prescaler (CPSDVSR). See SSI_CLOCKING.md
     * 
     * This value must be specified and greater than 0. It must also be even.
     * Failing one of these conditions will trigger an internal assert in debug
     * mode. In production, this causes undefined behavior.
     */
    uint8_t ssiPrescaler;

    /**
     * @brief Serial clock rate modifier (SCR). See SSI_CLOCKING.md.
     * 
     * Default is 0.
     */
    uint8_t ssiClockModifier;

    /** 
     * @brief whether the SSI is either transmitting or receiving info.
     * 
     * Default is false (It is transmitting).
     */
    bool isReceiving;

    /** ------------- Optional Fields. ------------- */

    /** 
     * @brief What frame format of the SSI should be used. 
     * 
     * Default is FREESCALE_SPI.
     */
    enum SSIFrameFormat frameFormat;

    /** 
     * @brief Whether the SSI module is Primary or Secondary.
     * 
     * Default is false (It is the primary device).
     */
    bool isSecondary;

    /** 
     * @brief Whether the clock line is fixed low when not executing.
     * 
     * Default is false (Clock is high when not executing).
     */
    bool isClockLow;

    /** 
     * @brief Whether the data bits are captured on the 1st or 2nd edge. 
     *        True is 2nd edge, False is 1st edge.
     * 
     * Default is false (1st edge).
     */ 
    enum SSIPolarity { SSI_FIRST_EDGE, SSI_SECOND_EDGE } polarity;

    /**
     * @brief isLoopback is an internal testing mechanism to tie the TX shift
     *        register to the RX shift register. It is recommended to use an
     *        actual SSI device to test with, for now. Further usage
     *        for this mode may be provided at a later date.
     * 
     * Default is false (no loopback).
     */
    bool isLoopback;
} SSIConfig_t;

/**
 * @brief SSIInit initializes a given SSI module. 
 * 
 * @param SSIConfig Configuration details of a given SSI module.
 * @return A SSIModule_t that is used to communicate
 */
SSIModule_t SSIInit(SSIConfig_t SSIConfig);

/**
 * @brief SPIRead attempts to read the latest set of data in the internal
 *        buffer. Busy waits until the receive buffer is not empty.
 * 
 * @param ssi SSI to check buffer for.
 * @return Right justified 16-bit data in the SSI data register.
 */
uint16_t SPIRead(SSIModule_t ssi);

/**
 * @brief SPIWrite attempts to write data in the internal buffer. Busy
 *        waits until the transmit buffer is not full.
 * 
 * @param ssi SSI to write into.
 * @param data Right justified 16-bit data to write.
 */
void SPIWrite(SSIModule_t ssi, uint16_t data);
