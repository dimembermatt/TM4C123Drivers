//UART1.h
//Dung Nguyen
//Wally Guzman

#include <stdint.h> 

void UART1_Init(uint32_t baud);

//--------UART1_OutChar--------
// Prints a character to UART1
// Inputs: character to transmit
// Outputs: none
void UART1_OutChar(char data);
void UART1_OutString(char *pt);

//--------UART1_EnableRXInterrupt--------
// Enables UART1 RX interrupt
// Inputs: none
// Outputs: none
void UART1_EnableRXInterrupt(void);

//--------UART1_DisableRXInterrupt--------
// Disables UART1 RX interrupt
// Inputs: none
// Outputs: none
void UART1_DisableRXInterrupt(void);

//------------UART1_FinishOutput------------
// Wait for all transmission to finish
// Input: none
// Output: none
void UART1_FinishOutput(void);
