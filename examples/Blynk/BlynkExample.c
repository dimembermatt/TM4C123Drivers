/**
 * File name: BlynkExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate outputting audio from a speaker.
 * Authors: Matthew Yu.
 * Last Modified: 03/04/21
 */

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../inc/esp8266.h"
#include "../lib/ST7735/ST7735.h"
#include "../lib/Timers/Timers.h"
#include "../lib/Blynk/Blynk.h"


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

uint32_t count = 0;
void sendInformation(void) {
    TM4CtoBlynk(74, count);
    count = (count + 1) % 30;
}

static char buf[64] = { NULL };
uint8_t messageReceived = 0;
void retreiveInformation(void) {
    messageReceived = BlynktoTM4C(buf, 64);
}


/** Initializes both onboard switches to test triggers. */
int main(void) {
    PLL_Init(Bus80MHz);
    DisableInterrupts();

    ST7735Init();
    ST7735DrawString(0, 0, "EE445L Lab 4D Blynk", ST7735_WHITE, ST7735_BLACK);

    ESP8266_Init();
    ESP8266_Reset();
    ESP8266_SetupWiFi();

    ST7735DrawString(0, 10, "Wifi connected.", ST7735_WHITE, ST7735_BLACK);

    TimerInit(TIMER_0A, freqToPeriod(100, MAX_FREQ), retreiveInformation);
    TimerInit(TIMER_1A, freqToPeriod(2, MAX_FREQ), sendInformation);

	EnableInterrupts();

	while(1) {
        WaitForInterrupt();

            // NOTE: debug statements
        if (messageReceived) {
            char pinNumber[2] = "99";
            char pinInteger[8] = "0000";
            char pinFloat[9] = "0.0000";
            // Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
            // Pin #, Integer Value, Float Value.
            strcpy(pinNumber, strtok(buf, ","));
            strcpy(pinInteger, strtok(NULL, ","));          // Integer value that is determined by the Blynk App
            strcpy(pinFloat, strtok(NULL, ","));            // Not used
            uint32_t pinNum = atoi(pinNumber);              // Need to convert ASCII to integer
            uint32_t pinInt = atoi(pinInteger);  

            // ---------------------------- VP #1 ----------------------------------------
            // This VP is the LED select button
            if (pinNum == 0x01)  {  
                if (pinInt == 1) ST7735FillScreen(ST7735_CYAN);
                else ST7735FillScreen(ST7735_BLACK);
            }  
            messageReceived = 0;
        }
    }
}
