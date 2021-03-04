/**
 * File name: Blynk.h
 * Devices: LM4F120; TM4C123
 * Description: Drivers for sending data to and from the Blynk application using
 * the TM4C123 and ESP8266.
 * Authors: Authored by Mark McDermott, Andrew Lynch, Jonathan Valvano, and
 * adapted by Matthew Yu.
 * Last Modified: 03/04/21
 */
#pragma once

/** General imports. */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/** Device specific imports. */
#include "../../inc/tm4c123gh6pm.h"
#include "../../inc/PLL.h"
#include "../Timers/Timers.h"
#include "../../inc/UART.h" // TODO: convert into lib later.
#include "../../inc/esp8266.h" // TODO: convert into lib later.

/**
 * TM4CtoBlynk sends data to the Blynk app.
 * It uses Virtual Pin numbers 70 - 99 so that the ESP8266 knows how to forward
 * the data to the Blynk app.
 * @param pin Virtual pin number.
 * @param value Value to send.
 */
void TM4CtoBlynk(uint32_t pin, uint32_t value);

/**
 * BlynktoTM4C receives data from the ESP8266 in the form as a Virtual Pin, and
 * parses the data and feeds the command to the TM4C.
 * @param serialBuffer Buffer filled by result of a complete message, if applicable.
 * @param size size of the serialBuffer - the max value to return.
 * @return How many characters were filled in the buffer.
 * @note It is the obligation of the caller to clear the buffer after using it.
 */
uint32_t BlynktoTM4C(char serialBuffer[], uint32_t size);
