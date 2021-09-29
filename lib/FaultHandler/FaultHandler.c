/**
 * @file FaultHandler.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief This file contains user implementations of fault handlers such as
 *        HardFault_Handler, __aeabi_assert, and so on.
 * @version 0.1
 * @date 2021-09-28
 * @copyright Copyright (c) 2021
 */

/** Device imports. */
#include <lib/FaultHandler/FaultHandler.h>
#include <lib/GPIO/GPIO.h>

void DisableInterrupts(void);   // Defined in startup.s

void __aeabi_assert(const char * _expr __attribute__((unused)), const char * _file __attribute__((unused)), int _line __attribute__((unused))) {
    DisableInterrupts();
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

    /* TODO: Once UART is enabled, write to UART an error message. */

    while (1) {}
}

void HardFault_Handler(void) {
    DisableInterrupts();
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

    /* TODO: Once UART is enabled, write to UART an error message. */

    while (1) {}
}
