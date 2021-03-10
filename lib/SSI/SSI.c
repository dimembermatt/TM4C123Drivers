/**
 * SSI.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for SSI communication. Also known as SSI.
 * Authors: Matthew Yu.
 * Author: Matthew Yu
 * Last Modified: 03/10/21
 */

/** Device specific imports. */
#include "SSI.h"
#include <TM4C123Drivers/inc/RegDefs.h>
#include <TM4C123Drivers/lib/GPIO/GPIO.h>


/**
 * SSIInit initializes a given SSI module. 
 * @param SSIConfig Configuration details of a given SSI module.
 * @note Potentially add the following parameters:
 *          - Set whether SSInClk pin is high when no data is transferred (p. 969)
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
        {PIN_A2, PULL_DOWN, true,  true, 2, false},    /* SSI0Clk. */
        {PIN_A3, PULL_DOWN, true,  true, 2, false},    /* SSI0Fss. */
        {PIN_A4, PULL_DOWN, false, true, 2, false},    /* SSI0Rx.  */
        {PIN_A5, PULL_DOWN, true,  true, 2, false},    /* SSI0Tx.  */
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
    for (uint8_t i = 0; i < 4; i++) {
        GPIOInit(configs[i]);
    }

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

    /* 8. Set Frame Format and N-bit data size. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CR0_OFFSET) &= ~(0x0000FFFF);
    GET_REG(SSI_BASE + SSIOffset + SSI_CR0_OFFSET) |= 
        (SSIConfig.frameFormat << 4) | (SSIConfig.dataBitSize-1);
    
    /* 9. Re-enable SSI operation. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CR1_OFFSET) |= 0x00000002;
}

void SSIClose(void) {}

void SPIRead(void) {}

void SPIWrite(void) {}
