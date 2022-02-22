/**
 * @file FaultHandler.h
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

#pragma once
#include <stdint.h>

/**
 * @brief __aeabi_assert is called by assert when it is violated by a bad debug
 *        conditions. It takes the assert debug capture at the assert failure,
 *        turns on PF1 (the red LED) and sends debug output to UART_MODULE_0. It
 *        then loops indefinitely with interrupts turned OFF.
 * 
 * @param expr The expression that caused the assert to fail.
 * @param file The file in which the assert failed.
 * @param line The line in which the assert failed.
 * @note Users are encouraged to override the implementation of __aeabi_assert
 *       in FaultHandler.c for their programs.
 */
void __aeabi_assert(const char *expr __attribute__((unused)), const char *file, int line) __attribute__((noreturn));

/**
 * @brief HardFault_Handler is executed when either: a developer calls this
 *        function directly, typically to highlight unfinished features or when
 *        the code enters an undefined state. This function will also be called
 *        by some WEAKly defined handlers in startup.s (or
 *        tm4c123gh6pm_startup_ccs.c) if the user has not implemented strong
 *        versions of those handlers.
 *        This function implementation turns on PF1 (the red LED) on the TM4C
 *        and loops indefinitely.
 * 
 * @note Users are encouraged to override the implementation of HardFault_Handler
 *       using this function definition.
 * @note The weak implementation of HardFault_Handler, however, provides basic 
 *       debug functionality, including catching the stack trace, turning on the
 *       red indicator LED, and printing the stack trace to PC via UART.
 */
void HardFault_Handler(void) __attribute__((noreturn));

/**
 * @brief _ReportHardFault is a function exposed to startup.s's weakly defined
 *        HardFault_Handler. It takes the stackframe at the time of faulting,
 *        turns on PF1 (the red LED), and sends debug output to UART_MODULE_0.
 *        It then loops indefinitely with interrupts turned OFF.
 * 
 * @param stackFrame Reference to registers that represent system state at time
 *                   of faulting.
 * @param _exc       Link register to the next point. Unused.
 */
void _ReportHardFault(uint32_t *stackFrame, uint32_t _exc);
