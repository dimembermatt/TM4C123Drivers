/**
 * @file FaultHandler.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief This file contains user implementations of fault handlers such as
 *        HardFault_Handler, __aeabi_assert, and so on.
 * @version 0.1
 * @date 2021-09-22
 * 
 * @copyright Copyright (c) 2021
 */

#pragma once


void __aeabi_assert(const char *expr, const char *file, int line);

void HardFault_Handler(void);
