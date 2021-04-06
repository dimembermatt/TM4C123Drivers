/**
 * File name: Blynk.c
 * Devices: LM4F120; TM4C123
 * Description: Drivers for sending data to and from the Blynk application using
 * the TM4C123 and ESP8266.
 * Authors: Authored by Mark McDermott, Andrew Lynch, Jonathan Valvano, and
 * adapted by Matthew Yu.
 * Last Modified: 03/13/21
 */

/** General imports. */
#include <string.h>
#include <stdlib.h>

/** Device specific imports. */
#include <inc/esp8266.h> // TODO: convert into lib later.
#include <lib/Blynk/Blynk.h>


/** Initializes the ESP8266 to communicate with the Blynk application. */
void BlynkInit(void) {
    ESP8266_Init();
    ESP8266_Reset();
    ESP8266_SetupWiFi();
}

/**
 * TM4CtoBlynk sends data to the Blynk app.
 * It uses Virtual Pin numbers 70 - 99 so that the ESP8266 knows how to forward
 * the data to the Blynk app.
 * @param pin Virtual pin number.
 * @param value Value to send.
 */
void TM4CtoBlynk(uint32_t pin, uint32_t value) {
    if (pin < 70 || pin > 99) return;

    // Format: [VP],[value],0.0\n
    ESP8266_OutUDec(pin);
    ESP8266_OutChar(',');
    ESP8266_OutUDec(value);
    ESP8266_OutChar(',');
    ESP8266_OutString("0.0\n");
}

/**
 * BlynktoTM4C receives data from the ESP8266 in the form as a Virtual Pin, and
 * parses the data and feeds the command to the TM4C.
 * @param serialBuffer Buffer filled by result of a complete message, if applicable.
 * @param size size of the serialBuffer - the max value to return.
 * @return How many characters were filled in the buffer.
 * @note It is the obligation of the caller to clear the buffer after using it.
 * TODO: update ESP8266 methods to return num chars copied.
 */
uint32_t BlynktoTM4C(char serialBuffer[], uint32_t size) {
    if (ESP8266_GetMessage(serialBuffer)) return 1;
    return 0;
}
