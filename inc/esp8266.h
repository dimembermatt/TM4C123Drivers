// -------------------------------------------------------------------
// File name:     esp8266.c
// Description:   This starter code is used to bridge the TM4C123 board
//                and the Blynk Application via the ESP8266. 
//
// Authors:       Mark McDermott
// Converted to EE445L style Jonathan Valvano
// Date:          May 21, 2018
// Last update: Sept 19, 2018
//
// 
//

#ifndef _ESP8266H_
#define _ESP8266H_
#include <stdint.h>
// comment out either/both to remove debugging
#define DEBUG1                // UART0 Debug output
#define DEBUG3                // ST7735 Debug output

// Initialize PE5,4,3,1,0 for interface to ESP8266
// Uses interrupt driven UART5 on PE5,4
// Uses simple GPIO output on PE3,1,0
void ESP8266_Init(void);

void ESP8266_Reset(void);

// ----------------------------------------------------------------------
// This routine sets up the Wifi connection between the TM4C and the
// hotspot. Enable the DEBUG flags if you want to watch the transactions.
void ESP8266_SetupWiFi(void);



// remove a message from front of index FIFO
// datapt points to an empty data buffer of MESSAGESIZE character
// if successful a message is copied from the FIFO into data buffer
// return FIFOSUCCESS if successful
// return FIFOFAIL if the FIFO is empty (no messages)
int ESP8266_GetMessage(char *datapt);



// output ASCII character to UART5, interrupt driven
// spin if Tx5Fifo is full
void ESP8266_OutChar(char data);

//------------ESP8266_OutString------------
// Output String (NULL termination)
// interrupt-driven, spin on Tx5Fifo full
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void ESP8266_OutString(char *pt);

//-----------------------ESP8266_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void ESP8266_OutUDec(uint32_t n);

#endif
  


