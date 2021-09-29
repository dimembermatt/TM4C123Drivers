/**
 * @file SSI.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief SSI (SPI) peripheral driver.
 * @version 0.1
 * @date 2021-09-23
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support DMA control. This driver
 * does not support interrupts.
 */

/** General imports. */
#include <assert.h>

/** Device specific imports. */
#include <inc/RegDefs.h>
#include <lib/SSI/SSI.h>
#include <lib/GPIO/GPIO.h>

SSIModule_t SSIInit(SSIConfig_t config) {
    /* Initialization asserts. */
    assert(config.ssi != 4 && config.ssi <= SSI1_PD);
    assert(SSI_SIZE_4 <= config.dataSize && config.dataSize <= SSI_SIZE_16);
    assert(0 < config.ssiPrescaler && !(config.ssiPrescaler & 0x1));
    assert(config.frameFormat <= MICROWIRE);
    assert(config.polarity <= SSI_SECOND_EDGE);

    /* 1. Enable the SSI module clock and stall until ready. */
    GET_REG(SYSCTL_BASE + SYSCTL_RCGCSSI_OFFSET) |= (1 << (config.ssi%4));
    while ((GET_REG(SYSCTL_BASE + SYSCTL_PRSSI_OFFSET) &
           (1 << (config.ssi%4))) == 0) {};

    /* 2. Enable the appropriate GPIO pins. */
    GPIOConfig_t configs[4] = {
        {PIN_A2, GPIO_PULL_UP,   true,  2, false, GPIO_DRIVE_2MA, false},    /* SSI0Clk. */
        {PIN_A3, GPIO_PULL_DOWN, true,  2, false, GPIO_DRIVE_2MA, false},    /* SSI0Fss. */
        {PIN_A4, GPIO_PULL_UP,   false, 2, false, GPIO_DRIVE_2MA, false},    /* SSI0Rx.  */
        {PIN_A5, GPIO_PULL_UP,   true,  2, false, GPIO_DRIVE_2MA, false},    /* SSI0Tx.  */
    };
    switch (config.ssi) {
        case SSI2_PB:
            configs[0].pin = PIN_B4;
            configs[1].pin = PIN_B5;
            configs[2].pin = PIN_B6;
            configs[3].pin = PIN_B7;
            break;
        case SSI1_PD:
            configs[0].pin = PIN_D0;
            configs[1].pin = PIN_D1;
            configs[2].pin = PIN_D2;
            configs[3].pin = PIN_D3;
            break;
        case SSI3_PD:
            configs[0].pin = PIN_D0;
            configs[1].pin = PIN_D1;
            configs[2].pin = PIN_D2;
            configs[3].pin = PIN_D3;
            configs[0].alternateFunction = 1;
            configs[1].alternateFunction = 1;
            configs[2].alternateFunction = 1;
            configs[3].alternateFunction = 1;
            break;
        case SSI1_PF:
            configs[0].pin = PIN_F0;
            configs[1].pin = PIN_F1;
            configs[2].pin = PIN_F2;
            configs[3].pin = PIN_F3;
            break;
        default:
            break;
    }
    GPIOInit(configs[0]);
    GPIOInit(configs[1]);

    /* Note Initialize the RX or TX pin depending on whether we're transmitting or not.
       If you think that you can keep both pins initialized, well, have fun for the
       next five hours wondering why your SSI DR is never written into. */
    /*if (config.isReceiving)*/ GPIOInit(configs[2]);
    /*else*/ GPIOInit(configs[3]);

    /* 3. We'll generate the SSI offset to find the correct addresses for each
       SSI module. */
    uint32_t SSIOffset = 0x1000 * (config.ssi%4);

    /* 4. Disable SSI operation. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CR1_OFFSET) &= ~0x0000000F;

    /* 5. Set SSI to primary/secondary and set loopback mode. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CR1_OFFSET) &= ~0x1F;
    GET_REG(SSI_BASE + SSIOffset + SSI_CR1_OFFSET) |=
        /* TODO: (config.endOfTransmit << 4) | */
        (config.isSecondary << 2) |
        (config.isLoopback);

    /* 6. Configure SSI clock source. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CC_OFFSET) = 0x00000000;

    /* 7. Configure clock prescale divisor. By default, 10MHz with an 80MHz
       SysClk. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CPSR_OFFSET) = config.ssiPrescaler;

    /* 8. Set clock phase, clock polarity, frame format, and data size. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CR0_OFFSET) &= ~(0x0000FFFF);
    GET_REG(SSI_BASE + SSIOffset + SSI_CR0_OFFSET) |=
        (config.ssiClockModifier << 8) |
        (config.polarity << 7) |
        (!config.isClockLow << 6) |
        (config.frameFormat << 4) |
        config.dataSize;

    /* 9. Re-enable SSI operation. */
    GET_REG(SSI_BASE + SSIOffset + SSI_CR1_OFFSET) |= 0x00000002;

    return config.ssi;
}

/**
 * See L157 of ST7735.c for extra insight on how SSI0 is managed,
 * as well as p. 974 of the TM4C datasheet.
 */

uint16_t SPIRead(SSIModule_t ssi) {
    /* We'll generate the SSI offset to find the correct addresses for each
       SSI module. */
    uint32_t SSIOffset = 0x1000 * (ssi%4);

    /* Poll until Receive FIFO is not empty. */
    while (!(GET_REG(SSI_BASE + SSIOffset + SSI_SR_OFFSET) & 0x4)) {}

    return GET_REG(SSI_BASE + SSIOffset + SSI_DR_OFFSET) & 0xFFFF;
}

void SPIWrite(SSIModule_t ssi, uint16_t data) {
    /* We'll generate the SSI offset to find the correct addresses for each
       SSI module. */
    uint32_t SSIOffset = 0x1000 * (ssi%4);

    /* Poll until Transmit FIFO is not full. */
    while (!(GET_REG(SSI_BASE + SSIOffset + SSI_SR_OFFSET) & 0x2)) {}
    GET_REG(SSI_BASE + SSIOffset + SSI_DR_OFFSET) = data;
}
