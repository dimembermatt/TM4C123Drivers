/**
 * @file UART.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Low level drivers to facilitate UART/serial communication.
 * @version 0.1
 * @date 2021-10-18
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support interrupts. This driver
 * does not support uDMA. This driver does not support modem handshake support.
 */

/** General Imports. */
#include <stdlib.h>
#include <assert.h>

/** Device specific imports. */
#include <inc/RegDefs.h>
#include <lib/UART/UART.h>
#include <lib/GPIO/GPIO.h>


static GPIOPin_t UARTPinMapping[9][2] = {
    // RX       // TX
    {PIN_A0, PIN_A1},
    {PIN_B0, PIN_B1},
    {PIN_D6, PIN_D7},
    {PIN_C6, PIN_C7},
    {PIN_C4, PIN_C5},
    {PIN_E4, PIN_E5},
    {PIN_D4, PIN_D5},
    {PIN_E0, PIN_E1}
};

static uint32_t UARTSpeedMapping[8][2] = {
    {520,  53}, // 9600
    {4166, 43}, // 1200
    {2083, 21}, // 2400
    {1041, 43}, // 4800
    {260,  27}, // 19200
    {130,  13}, // 38400
    {86,   52}, // 57600
    {43,   26}  // 115200
};

UART_t UARTInit(UARTConfig_t config) {
    /* Initialization asserts. */
    assert(config.module <= UART_MODULE_7);
    assert(config.baudrate <= UART_BAUD_115200);
    assert(config.dataLength <= UART_BITS_7);
    assert(config.parity <= UART_PARITY_EVEN);

    /* 1. Activate the clock for RCGCUART and stall until ready. */
    GET_REG(SYSCTL_BASE + SYSCTL_RCGCUART_OFFSET) |= 1 << config.module;
    while ((GET_REG(SYSCTL_BASE + SYSCTL_PRUART_OFFSET) &
           (1 << config.module)) == 0) {};

    /* 2. Enable RX and TX GPIO pins. */
    GPIOConfig_t configs[2] = {
        {UARTPinMapping[config.module][0], GPIO_TRI_STATE, false, 1, false, GPIO_DRIVE_2MA, false},
        {UARTPinMapping[config.module][1], GPIO_TRI_STATE, true, 1, false, GPIO_DRIVE_2MA, false}
    };
    GPIOInit(configs[0]);
    GPIOInit(configs[1]);

    /* 3. We'll generate the UART base to find the correct addresses for each
          UART module. */
    uint32_t moduleBase = 0x1000 * config.module + UART_BASE;

    /* 4. Disable UART operation. */
    GET_REG(moduleBase + UART_CTL_OFFSET) &= 0xFFFE;

    /* 5. Set baud rate. */
    GET_REG(moduleBase + UART_IBRD_OFFSET) = UARTSpeedMapping[config.baudrate][0];
    GET_REG(moduleBase + UART_FBRD_OFFSET) = UARTSpeedMapping[config.baudrate][1];

    /* 6. Set data length, fifo enabled, stop bits, and parity. */
    GET_REG(moduleBase + UART_LCRH_OFFSET) &= ~(0x0000FFFF);
    /* A clever bit of math here to keep UART_BITS_8 the default initializer
       enum value but at the same time 0x3 in the LCRH register.

       (8bit) 0 + 3 = 3 % 4 = 3
       (5bit) 1 + 3 = 4 % 4 = 0
       (6bit) 2 + 3 = 5 % 4 = 1
       (7bit) 3 + 3 = 6 % 4 = 2 */
    GET_REG(moduleBase + UART_LCRH_OFFSET) |=
        (((config.dataLength + 3) % 4) << 5) |
        (!config.isFIFODisabled << 4) |
        (config.isTwoStopBits << 3) |
        ((config.parity == UART_PARITY_EVEN) << 2) |
        ((config.parity > UART_PARITY_DISABLED) << 1);

    /* 7. Enable UART TX and RX, and loopback mode, if necessary. */
    GET_REG(moduleBase + UART_CTL_OFFSET) |=
        (1 << 9) |
        (1 << 8) |
        (config.isLoopback << 7);

    /* 8. Set clock source. */
    GET_REG(moduleBase + UART_CC_OFFSET) = 0; // System clock.

    /* 9. Re-enable UART operation. */
    GET_REG(moduleBase + UART_CTL_OFFSET) |= 0x0001;

    UART_t uart = {
        .module=config.module
    };

    return uart;
}

uint8_t UARTSend(UART_t uart, uint8_t * values, uint8_t numValues) {
    uint32_t moduleBase = 0x1000 * uart.module + UART_BASE;

    uint8_t i;
    for (i = 0; i < numValues; ++i) {
        /* 1. Wait for UART ready (while Transmit FIFO full). */
        while ((GET_REG(moduleBase + UART_FR_OFFSET) & 0x20) != 0);

        /* 2. Write to DR. */
        GET_REG(moduleBase + UART_DR_OFFSET) = values[i];
    }

    return i;
}

uint8_t UARTReceive(UART_t uart, uint8_t * values, uint8_t maxNumValues) {
    uint32_t moduleBase = 0x1000 * uart.module + UART_BASE;

    if (GET_REG(moduleBase + UART_FR_OFFSET) & 0x10) return false;

    uint8_t i = 0;
    while (i < maxNumValues) {
        /* 1. Check for UART ready (if Receive FIFO is empty, early exit). */
        if (GET_REG(moduleBase + UART_FR_OFFSET) & 0x10) break;

        /* 2. Retrieve from DR. */
        values[i] = GET_REG(moduleBase + UART_DR_OFFSET);

        ++i;
    }
    return i;
}
