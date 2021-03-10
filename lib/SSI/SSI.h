/**
 * SSI.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for SSI communication.
 * Authors: Matthew Yu.
 * Author: Matthew Yu
 * Last Modified: 03/10/21
 */
#pragma once

/** General imports. */
#include <stdint.h>
#include <stdbool.h>


enum SSISelect{     /** Clk, Fss, Rx,  Tx.  */
    SSI0_PA=0,      /** PA2, PA3, PA4, PA5. */
    SSI1_PF=1,      /** PF2, PF3, PF0, PF1. */
    SSI2_PB=2,      /** PB4, PB5, PB6, PB7. */
    SSI3_PD=3,      /** PD0, PD1, PD2, PD3. */
    UNDEFINED=4,    /** Invalid SSI Config. */
    SSI1_PD=5,      /** PD0, PD1, PD2, PD3. */
};

enum SSIFrameFormat{
    FREESCALE_SPI,
    TI_SYNCHRONOUS_SERIAL,
    MICROWIRE,
    RESERVED
};

typedef struct SSIConfig {
    /** Which SSI module and port. */
    enum SSISelect SSI;

    /** Determines the specific frame format of the SSI. */
    enum SSIFrameFormat frameFormat;

    /** Determines whether the SSI module is Primary or Secondary. */
    bool isPrimary;

    /** The size of the SSI. From 4 (0x3) to 16 (0xF) bits. */
    uint8_t dataBitSize;
} SSIConfig_t;

void SSIInit(SSIConfig_t SSIConfig);

void SSIClose(void);

void SPIRead(void);

void SPIWrite(void);
