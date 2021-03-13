/**
 * SSI.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for SSI communication. Also known as SSI.
 * Authors: Matthew Yu.
 * Author: Matthew Yu
 * Last Modified: 03/13/21
 */

/** Device specific imports. */
#include "SSI.h"
#include <TM4C123Drivers/inc/RegDefs.h>
#include <TM4C123Drivers/lib/GPIO/GPIO.h>


/**
 * SSIInit initializes a given SSI module. 
 * @param SSIConfig Configuration details of a given SSI module.
 * @note Potentially add the following parameters:
 *          - Set whether SSI loopback mode is enabled (p. 972)
 *          - Add parameter in SSIConfig_t for bit rate and SysClk speed.
 *          Determine appropriate prescaler and bit rate selection in SSICR0.
 */
void SSIInit(SSIConfig_t SSIConfig) {
    /* Early return on invalid SSI config. */
    if (SSIConfig.SSI == UNDEFINED) return;

    /* Early return if invalid bit size. */
    if (SSIConfig.dataBitSize < 0x3 && SSIConfig.dataBitSize > 0xF) return;

    /* 1. Enable the SSI module clock. */
    GET_REG(SYSCTL_BASE + SYSCTL_RCGCSSI_OFFSET) |= (1 << (SSIConfig.SSI%4));
    while ((GET_REG(SYSCTL_BASE + SYSCTL_PRSSI_OFFSET) & 
           (1 << (SSIConfig.SSI%4))) == 0) {};

    /* 2. Enable the appropriate GPIO pins. */
    GPIOConfig_t configs[4] = {
        {PIN_A2, PULL_UP,   true,  true, 2, false},    /* SSI0Clk. */
        {PIN_A3, PULL_DOWN, true,  true, 2, false},    /* SSI0Fss. */
        {PIN_A4, PULL_UP,   false, true, 2, false},    /* SSI0Rx.  */
        {PIN_A5, PULL_UP,   true,  true, 2, false},    /* SSI0Tx.  */
    };
    switch(SSIConfig.SSI) {
        case SSI2_PB:
            configs[0].GPIOPin = PIN_B4;
            configs[1].GPIOPin = PIN_B5;
            configs[2].GPIOPin = PIN_B6;
            configs[3].GPIOPin = PIN_B7;
            break;
        case SSI1_PD:
            configs[0].GPIOPin = PIN_D0;
            configs[1].GPIOPin = PIN_D1;
            configs[2].GPIOPin = PIN_D2;
            configs[3].GPIOPin = PIN_D3;
            break;
        case SSI3_PD:
            configs[0].GPIOPin = PIN_D0;
            configs[1].GPIOPin = PIN_D1;
            configs[2].GPIOPin = PIN_D2;
            configs[3].GPIOPin = PIN_D3;
            configs[0].alternateFunction = 1;
            configs[1].alternateFunction = 1;
            configs[2].alternateFunction = 1;
            configs[3].alternateFunction = 1;
            break;
        case SSI1_PF:
            configs[0].GPIOPin = PIN_F0;
            configs[1].GPIOPin = PIN_F1;
            configs[2].GPIOPin = PIN_F2;
            configs[3].GPIOPin = PIN_F3;
            break;
        default:
            break;
    }
    GPIOInit(configs[0]);
    GPIOInit(configs[1]);

    /* Initialize RX or TX pin depending on whether we're transmitting or not.
     * If you think you can keep both pins initialized, well, have fun for the
     * next five hours wondering why your SSI DR is never written into. */ 
    if (!SSIConfig.isTransmitting) GPIOInit(configs[2]);
    else GPIOInit(configs[3]);

    /* 3. We'll generate the SSI offset to find the correct addresses for each
     * SSI module. */
    uint32_t SSIOffset = 0x1000 * (SSIConfig.SSI%4);

    /* 4. Disable SSI operation. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CR1_OFFSET) &= ~0x00000002;

    /* 5. Set SSI to primary/secondary. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CR1_OFFSET) |= (!SSIConfig.isPrimary << 2);

    /* 6. Configure SSI clock source. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CC_OFFSET) = 0x00000000;

    /* 7. Configure clock prescale divisor. By default, 10MHz with an 80MHz
     * SysClk. */ 
    GET_REG(SSI_BASE + SSIOffset + SSI_CPSR_OFFSET) = 0x00000008;

    /* 8. Set Frame Format and N-bit data size. Also, force steady high when no
       data is transferred. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CR0_OFFSET) &= ~(0x0000FFFF);
    GET_REG(SSI_BASE + SSIOffset + SSI_CR0_OFFSET) |= 
        (SSIConfig.frameFormat << 4) | 
        (0x1 << 6) | 
        (SSIConfig.dataBitSize-1);

    /* 9. Re-enable SSI operation. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CR1_OFFSET) |= 0x00000002;
}

/**
 * See L157 of ST7735.c for extra insight on how SSI0 is managed,
 * as well as p. 974 of the TM4C datasheet.
 */

/**
 * SPIRead attempts to read the latest set of data in the internal buffer. Busy
 * waits until the receive buffer is not empty.
 * @param ssi SSI to check buffer for.
 * @return Right justified 16-bit data in the SSI data register.
 */
uint16_t SPIRead(enum SSISelect ssi) {
    /* We'll generate the SSI offset to find the correct addresses for each
     * SSI module. */
    uint32_t SSIOffset = 0x1000 * (ssi%4);

    /* Poll until Receive FIFO is not empty. */
    while ((GET_REG(SSI_BASE + SSIOffset + SSI_SR_OFFSET) & 0x4) == 0) {}
    return GET_REG(SSI_BASE + SSIOffset + SSI_DR_OFFSET) & 0x00FF;
}

/**
 * SPIWrite attempts to write data in the internal buffer. Busy
 * waits until the transmit buffer is not full.
 * @param ssi SSI to write into.
 * @param data Right justified 16-bit data to write.
 */
void SPIWrite(enum SSISelect ssi, uint16_t data) {
    /* We'll generate the SSI offset to find the correct addresses for each
     * SSI module. */
    uint32_t SSIOffset = 0x1000 * (ssi%4);

    /* Poll until Transmit FIFO is not full. */
    while ((GET_REG(SSI_BASE + SSIOffset + SSI_SR_OFFSET) & 0x2) == 0) {}
    GET_REG(SSI_BASE + SSIOffset + SSI_DR_OFFSET) = data;
    /* TODO: 3/13/21 to future Matthew: add back in DisableInterrupts and
     * EnableInterrupts here if Lab5 is still broken. We know that SSI must
     * work... */
}
