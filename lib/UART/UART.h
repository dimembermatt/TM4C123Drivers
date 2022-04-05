/**
 * @file UART.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Low level drivers to facilitate UART/serial communication.
 * @version 0.1
 * @date 2021-10-18
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support interrupts. This driver
 * does not support uDMA. This driver does not support modem handshake support.
 */

#pragma once

/** General imports. */
#include <stdint.h>
#include <stdbool.h>


/**
 * @brief UARTModule is an enumeration specifying a set of pins used for UART
 *        communication. */
enum UARTModule {
    UART_MODULE_0,  // PA0 RX
                    // PA1 TX

    UART_MODULE_1,  // PB0 RX
                    // PB1 TX

    UART_MODULE_2,  // PD6 RX
                    // PD7 TX

    UART_MODULE_3,  // PC6 RX
                    // PC7 TX

    UART_MODULE_4,  // PC4 RX
                    // PC5 TX

    UART_MODULE_5,  // PE4 RX
                    // PE5 TX

    UART_MODULE_6,  // PD4 RX
                    // PD5 TX

    UART_MODULE_7   // PE0 RX
                    // PE1 TX
};

/**
 * @brief UARTBaudrate is an enumeration specifying a set of baud rates used for
 *        UART communication. */
enum UARTBaudrate {
    UART_BAUD_9600,
    UART_BAUD_1200,
    UART_BAUD_2400,
    UART_BAUD_4800,
    UART_BAUD_19200,
    UART_BAUD_38400,
    UART_BAUD_57600,
    UART_BAUD_115200
};

/**
 * @brief UARTDataLength is an enumeration specifying the length of the data
 *        sent in the data frame. */
enum UARTDataLength {
    UART_BITS_8,
    UART_BITS_5,
    UART_BITS_6,
    UART_BITS_7
};

/**
 * @brief UARTParity is an enumeration specifying the parity checking by the MCU
 *        when transmitting or receiving packets. */
enum UARTParity {
    UART_PARITY_DISABLED,
    UART_PARITY_ODD,
    UART_PARITY_EVEN
};

/**
 * @brief UARTConfig_t is a user defined struct that specifies an UART config.
 */
typedef struct UARTConfig {
    /**
     * @brief The UART module used for configuration.
     * 
     * Default is UART_MODULE_0 (PA0, PA1).
     */
    enum UARTModule module;

    /** ------------- Optional Fields. ------------- */

    /**
     * @brief The baudrate used in communication with the received device.
     * 
     * Default is 9600 bits per second.
     */
    enum UARTBaudrate baudrate;

    /**
     * @brief The length of the data in a frame.
     * 
     * Default is 8 bits (1 char) per frame transmission.
     */
    enum UARTDataLength dataLength;

    /**
     * @brief Whether the internal FIFO is disabled or not.
     * 
     * Default enabled (false). Otherwise, the FIFOs become 1-byte-deep holding
     * registers. 
     */
    bool isFIFODisabled;

    /**
     * @brief Whether the frame contains 1 or 2 stop bits.
     * 
     * Default is false. There is only a single stop bit transmitted at the end
     * of a frame.
     */
    bool isTwoStopBits;

    /**
     * @brief Whether the UART uses even, odd, or no parity at all.
     * 
     * Default no parity.
     */
    enum UARTParity parity;

    /**
     * @brief Whether the UART is in loopback mode. This is typically used for
     * testing. 
     * 
     * Default false, the UART is not in loopback mode.
     */
    bool isLoopback;
} UARTConfig_t;

/**
 * @brief UART_t is a struct containing user relevant data of a UART peripheral.
 */
typedef struct UART {
    /** @brief The UART module used for communicating with another device. */
    enum UARTModule module;
} UART_t;

/**
 * @brief UARTInit initializes a UART module given an UARTConfig_t
 * configuration. 
 * 
 * @param config The configuration of the UART module.
 * @return An UART_t struct instance used for communication.
 */
UART_t UARTInit(UARTConfig_t config);

/**
 * @brief UARTSend transmits a specified number of values across the data line of
 *        a provided UART.
 * 
 * @param uart The UART module to transmit across.
 * @param values Pointer to the data array to send.
 * @param numValues The number of values in the data array to send.
 * @return The number of values transmitted successfully.
 */
uint8_t UARTSend(UART_t uart, uint8_t * values, uint8_t numValues);

/**
 * @brief UARTReceive receives a variable number of values from the receive FIFO.
 *        
 * @param uart The UART module to receive across.
 * @param values Pointer to the data array to fill.
 * @param numValues The number of values in the data array to fill.
 * @return The number of values received.
 */
uint8_t UARTReceive(UART_t uart, uint8_t * values, uint8_t numValues);
