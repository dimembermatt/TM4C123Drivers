/**
 * I2C.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for I2C management.
 * Authors: Matthew Yu.
 * Last Modified: 09/20/21
 */

/** General Imports. */
#include <stdlib.h>
#include <stdbool.h>

/** Device specific imports. */
#include <inc/RegDefs.h>
#include <lib/I2C/I2C.h>

void HardFault_Handler(void);    // Defined in startup.s

static pin_t I2CPinMapping[4][2] = {
    //SCL    //SDA
    {PIN_B2, PIN_B3},
    {PIN_A6, PIN_A7},
    {PIN_E4, PIN_E5},
    {PIN_D0, PIN_D1},
};

static uint32_t I2CSpeedMapping[4] = {
    100000,
    400000,
    1000000,
    3330000
};

I2C_t I2CInit(I2CConfig_t i2cConfig) {
    /* 1. Enable SCL and SDA GPIO pins. */
    GPIOConfig_t sclPin = {
        I2CPinMapping[i2cConfig.module][0],
        NONE, /* Do NOT configure SCL as open drain. */
        true,
        true,
        3,
        false
    };
    GPIOInit(sclPin);
    GPIOConfig_t sdaPin = {
        I2CPinMapping[i2cConfig.module][1],
        OPEN_DRAIN,
        true,
        true,
        3,
        false
    };
    GPIOInit(sdaPin);

    /* 2. Activate the clock for RCGCI2C. */
    GET_REG(SYSCTL_BASE + SYSCTL_RCGCI2C_OFFSET) |=
        1 << i2cConfig.module;

    /* 3. Stall until clock is ready. */
    while ((GET_REG(SYSCTL_BASE + SYSCTL_PRI2C_OFFSET) &
        (1 << i2cConfig.module)) == 0) {};

    uint32_t moduleBase = i2cConfig.module * 1000 + I2C0_BASE;

    /* 4. Set transmission mode. */
    /* Glitch filter is disabled, master mode enabled, and loopback mode is on. */
    GET_REG(moduleBase + I2C_MCR_OFFSET) = 0x11;

    // if in master mode.
    /* 5. Select clock speed. */
    /* Default 80 MHz. */
    uint8_t tpr = 80000000 / (20 * I2CSpeedMapping[i2cConfig.speed]) - 1;
    GET_REG(moduleBase + I2C_MTPR_OFFSET) = tpr;

    I2C_t i2c = {
        i2cConfig.module,
        (i2cConfig.I2CErrorHandler != NULL) ? i2cConfig.I2CErrorHandler : &HardFault_Handler
    };

    return i2c;
}

void I2CMasterTransmit(I2C_t i2c, uint8_t slaveAddress, uint8_t * bytes, uint8_t numBytes) {
    uint32_t moduleBase = i2c.module * 1000 + I2C0_BASE;

    /* 1. Write slave address to I2CMSA. */
    GET_REG(moduleBase + I2C_MSA_OFFSET) = (slaveAddress << 1) & 0xFE; // last bit is 0 for transmit.

    /* 2. Write data to I2CMDR. */
    GET_REG(moduleBase + I2C_MDR_OFFSET) = bytes[0];
    
    /* 3. Busy wait on I2CMCS bus. */
    while ((GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x40)) {}

    /* 4. Write start and run bits to I2CMCS. */
    GET_REG(moduleBase + I2C_MCS_OFFSET) = 0x03;

    /* 5. For remaining bytes. */
    for (uint8_t i = 1; i < numBytes; ++i) {
        /* 5.1. Busy wait on I2CMCS. */
        while ((GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x01)) {}

        /* 5.2. Check for error. */
        if (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x02) {
            if (!(GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x10)) {
                GET_REG(moduleBase + I2C_MCS_OFFSET) = 0x04;
            }
            i2c.I2CErrorHandler();
            return;
        }

        /* 5.3. Write data to I2CMDR. */
        GET_REG(moduleBase + I2C_MDR_OFFSET) = bytes[i];

        /* 5.4. If there are bytes left to write, write the run bit to I2CMCS. */
        if (i < numBytes - 1) {
            GET_REG(moduleBase + I2C_MCS_OFFSET) = 0x01;
        } 
    }

    /* 6. When done, write stop and run to I2CMCS. */
    GET_REG(moduleBase + I2C_MCS_OFFSET) = 0x05;

    /* 7. Busy wait on I2CMCS. */
    while ((GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x01)) {}

    /* 8. On error, got to error service. */
    if (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x02) {
        i2c.I2CErrorHandler();
    }
}

void I2CMasterReceive(I2C_t i2c, uint8_t slaveAddress, uint8_t bytes[], uint8_t numBytes) {
    uint32_t moduleBase = i2c.module * 1000 + I2C0_BASE;

    /* 1. Write slave address to I2CMSA. */
    GET_REG(moduleBase + I2C_MSA_OFFSET) = (slaveAddress << 1) | 0x1; // last bit is 1 for receive.

    /* 2. Busy wait on I2CMCS bus. */
    while ((GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x40)) {}

    /* 3. Write ack, start, and run bits to I2CMCS. */
    GET_REG(moduleBase + I2C_MCS_OFFSET) = 0x0B;

    /* 4. For all bytes. */
    for (uint8_t i = 0; i < numBytes; ++i) {
        /* 4.1. Busy wait on I2CMCS. */
        while ((GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x01)) {}

        /* 4.2. Check for error. */
        if (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x02) {
            if (!(GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x10)) {
                GET_REG(moduleBase + I2C_MCS_OFFSET) = 0x04;
            }
            i2c.I2CErrorHandler();
            return;
        }

        /* 4.3. Read data from I2CMDR. */
        bytes[i] = GET_REG(moduleBase + I2C_MDR_OFFSET);

        /* 5.4. If there are bytes left to write, write the run bit to I2CMCS. */
        if (i < numBytes - 1) {
            GET_REG(moduleBase + I2C_MCS_OFFSET) = 0x11;
        } 
    }

    /* 5. When done, write ack and run bits to I2CMCS. */
    GET_REG(moduleBase + I2C_MCS_OFFSET) = 0x05;

    /* 6. Busy wait on I2CMCS. */
    while ((GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x01)) {}

    /* 7. On error, got to error service. */
    if (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x02) {
        i2c.I2CErrorHandler();
    }
}

void I2CSlaveProcess(I2C_t i2c, uint8_t slaveAddress, void (*readTask)(uint8_t), uint8_t (*writeTask)(void)) {
    uint32_t moduleBase = i2c.module * 1000 + I2C0_BASE;

    /* Write own slave address to I2CSOAR. */
    GET_REG(moduleBase + I2C_SOAR_OFFSET) = slaveAddress << 1;

    /* Write to device activate bit I2CSCSR. */
    GET_REG(moduleBase + I2C_SCSR_OFFSET) = 1;

    /* Determine Read request or write request. */
    while (!(GET_REG(moduleBase + I2C_SCSR_OFFSET) & 0x03)) {}
    if (GET_REG(moduleBase + I2C_SCSR_OFFSET) & 0x01) {
        /* Upon read request, read data from I2CSDR. */
        uint8_t data = GET_REG(moduleBase + I2C_SDR_OFFSET) & 0xFF;
        if (readTask != NULL)
            readTask(data);
    } else if (GET_REG(moduleBase + I2C_SCSR_OFFSET & 0x02)) {
        /* Upon transmit request, write data to I2CSDR. */

        if (writeTask != NULL) {
            uint8_t data = writeTask();
            GET_REG(moduleBase + I2C_SDR_OFFSET) = data;
        } else {
            GET_REG(moduleBase + I2C_SDR_OFFSET) = 0xFF;
        }
    } else {
        /* Illegal state. Goto error handler. */
        i2c.I2CErrorHandler();
    }
}
