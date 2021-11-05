/**
 * @file FaultHandler.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief This file contains user implementations of fault handlers such as
 *        HardFault_Handler, __aeabi_assert, and so on.
 * @version 0.1
 * @date 2021-11-05
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

void __aeabi_assert(const char * _expr __attribute__((unused)), const char * _file __attribute__((unused)), int _line __attribute__((unused))) {
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

    while (1) {}
}

void HardFault_Handler(void) {
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
    
    /* This delay is required to get the TM4C running. 
       If this is not included, then there's a 60% 
       chance that the UART debug output is garbage. */
    EnableInterrupts();
    DelayInit();
    DelayMillisec(10);
    
    /* Preamble. */
    char msg[34] = "\n\rRuntime Hardfault encountered.\n\r";
    UARTSend(uart, (uint8_t *) msg, 34); 
    
    DisableInterrupts();

    while (1) {}
}
