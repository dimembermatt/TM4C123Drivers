/**
 * @file FaultHandler.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief This file contains user implementations of fault handlers such as
 *        HardFault_Handler, __aeabi_assert, and so on.
 * @version 0.1
 * @date 2022-02-20
 * @copyright Copyright (c) 2021
 * @note There is a dependency on GPIO, UART, and Timer, but FaultHandler
 * doesn't need need to be added to the project if you don't want those
 * dependencies.
 */

/** General imports. */
#include <stdio.h>

/** Device imports. */
#include <lib/FaultHandler/FaultHandler.h>
#include <lib/GPIO/GPIO.h>
#include <lib/UART/UART.h>
#include <lib/Timer/Timer.h>


void DisableInterrupts(void);   // Defined in startup.s
void EnableInterrupts(void);    // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s
void HardfaultCapture(void);    // Defined in startup.s


/**
 * @brief _enableFaultState is a helper function for __aeabi_assert and
 *        _ReportHardFault that turns on the red LED indicator and initializes
 *        UART_MODULE_0 for talking to the PC (if connected).
 * 
 * @return UART_t connection to UART_MODULE_0.
 */
UART_t _enableFaultState(void) {
    /* Set red LED ON. */
    GPIOConfig_t PF1Config = {
        .pin=PIN_F1,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true,
        .alternateFunction=0,
        .isAnalog=false,
        .drive=GPIO_DRIVE_2MA,
        .enableSlew=false
    };

    GPIOPin_t PF1 = GPIOInit(PF1Config);
    GPIOSetBit(PF1, 1);

    /* Once UART is enabled, write to UART an error message. */
    UARTConfig_t config = {
        .module=UART_MODULE_0,
        .baudrate=UART_BAUD_9600,
        .dataLength=UART_BITS_8,
        .isFIFODisabled=false,
        .isTwoStopBits=false,
        .parity=UART_PARITY_DISABLED,
        .isLoopback=false
    };
    UART_t uart = UARTInit(config);

    /* This delay is required to get the TM4C running. If this is not included,
       then there's a 60% chance that the UART debug output is garbage. */
    EnableInterrupts();
    DelayInit();
    DelayMillisec(10);

    return uart;
}

void __aeabi_assert(const char * _expr __attribute__((unused)), const char * _file, int _line) {
    UART_t uart = _enableFaultState();

    /* Preamble. */
    char msg[31] = "\n\rASSERT FAILURE stack trace:\n\r";
    UARTSend(uart, (uint8_t *) msg, 31);
    /* Expression that the ASSERT failed on. */
    uint8_t i = 0;
    while (_expr[i] != '\0') ++i;
    UARTSend(uart, (uint8_t *)_expr, i);
    UARTSend(uart, (uint8_t *) (msg + 29), 2);
    /* File that the ASSERT failed on. */
    i = 0;
    while (_file[i] != '\0') ++i;
    UARTSend(uart, (uint8_t *)_file, i);
    UARTSend(uart, (uint8_t *) (msg + 29), 2);
    /* Line number that the ASSERT failed on. Up to 5 digits. */
    char line[5] = { '\0' };
    snprintf(line, 5, "%i", _line);
    UARTSend(uart, (uint8_t *) line, 5);

    DisableInterrupts();

    while (1) {
        WaitForInterrupt();
    }
}

void _ReportHardFault(uint32_t *stackFrame, uint32_t _exc) {
    #define NUM_FRAMES 8
    struct frameParam {
        char * name;
        uint32_t value;
    } frames[NUM_FRAMES] = {
        {"r0",  stackFrame[0]},
        {"r1",  stackFrame[1]},
        {"r2",  stackFrame[2]},
        {"r3",  stackFrame[3]},
        {"r12", stackFrame[4]},
        {"lr",  stackFrame[5]},
        {"pc",  stackFrame[6]},
        {"psr", stackFrame[7]},
    };

    UART_t uart = _enableFaultState();

    /* Preamble. */
    char msg[34] = "\n\rRuntime Hardfault encountered.\n\r";
    UARTSend(uart, (uint8_t *) msg, 34);

    #define BUFFER_SIZE 18
    char buffer[BUFFER_SIZE] = { '\0' };
    for (uint8_t i = 0; i < NUM_FRAMES; ++i) {
        for (uint8_t j = 0; j < BUFFER_SIZE; ++j) {
            buffer[j] = '\0';
        }
        /* Print parameter value at hardfault. */
        snprintf(
            buffer,
            BUFFER_SIZE,
            "%s:\t%#010x\n\r",
            frames[i].name,
            frames[i].value
        );
        UARTSend(uart, (uint8_t *)buffer, BUFFER_SIZE);
    }

    DisableInterrupts();

    while (1) {
        WaitForInterrupt();
    }

    #undef BUFFER_SIZE
    #undef NUM_FRAMES
}
