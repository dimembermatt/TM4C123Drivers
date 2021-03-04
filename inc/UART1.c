//UART1.c
//Dung Nguyen
//Wally Guzman

#include <stdint.h>
#include "../inc/UART1.h"
#include "../inc/tm4c123gh6pm.h"

void UART1_Init(uint32_t baud){
  volatile int delay;
    
    // Enable UART1
  SYSCTL_RCGCUART_R |= 0x02;
  delay++; delay++;
    
    // Enable PORT B clock gating
  SYSCTL_RCGCGPIO_R |= 0x02;
  delay++; delay++;
	
    // Enable UART1 
  GPIO_PORTB_AFSEL_R |= 0x03;
  GPIO_PORTB_DIR_R |= 0x20; // PB5 output to reset
  GPIO_PORTB_PCTL_R =(GPIO_PORTB_PCTL_R&0xFF0FFF00)|0x00000011;
  GPIO_PORTB_DEN_R   |= 0x23; //23 
  GPIO_PORTB_DATA_R |= 0x20; // reset high
  UART1_CTL_R &= ~UART_CTL_UARTEN;                  // Clear UART1 enable bit during config
	UART1_CTL_R |= 0x20;
// UART1_IBRD_R = 43;       // IBRD = int(80,000,000 / (16 * 115,200)) = int(43.403)
// UART1_FBRD_R = 26;       // FBRD = round(0.4028 * 64 ) = 26	
  UART1_IBRD_R = 5000000/baud;   										// IBRD = int(80,000,000 / (16 * baud))
  UART1_FBRD_R = ((64*(5000000%baud))+baud/2)/baud; // FBRD = round(remainder * 64 + 0.5)
	
//	UART1_IBRD_R = 86; //57,600
//	UART1_FBRD_R = 52;
	
  UART1_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);  // 8 bit word length, 1 stop, no parity, FIFOs enabled
  UART1_IFLS_R &= ~0x3F;                            // Clear TX and RX interrupt FIFO level fields
  UART1_IFLS_R += UART_IFLS_RX4_8;                  // RX FIFO interrupt threshold >= 1/2th full
  UART1_IM_R |= UART_IM_RXIM | UART_IM_RTIM;       	// Enable interupt on RX and RX transmission end
  UART1_CTL_R |= UART_CTL_UARTEN;                   // Set UART1 enable bit  

	//UART1_DMACTL_R |= 0x01;														// Enable DMA for RX FIFO
	
	NVIC_EN0_R = 1<<6;// Enable Interrupts 
}

//--------UART1_OutChar--------
// Prints a character to UART1
// Inputs: character to transmit
// Outputs: none
void UART1_OutChar(char data){
  while((UART1_FR_R&UART_FR_TXFF) != 0);
  UART1_DR_R = data;
}

void UART1_OutString(char *pt){
  while(*pt){
    UART1_OutChar(*pt);
    pt++;
  }
}

//--------UART1_EnableRXInterrupt--------
// - Enables UART1 RX interrupt
// Inputs: none
// Outputs: none
void UART1_EnableRXInterrupt(void){
  NVIC_EN0_R = 1<<6; // interrupt 6
}

//--------UART1_DisableRXInterrupt--------
// - Disables UART1 RX interrupt
// Inputs: none
// Outputs: none
void UART1_DisableRXInterrupt(void){
  NVIC_DIS0_R = 1<<6; // interrupt 6    
}

//------------UART1_FinishOutput------------
// Wait for all transmission to finish
// Input: none
// Output: none
void UART1_FinishOutput(void){
  // Wait for entire tx message to be sent
  // UART Transmit FIFO Empty =1, when Tx done
  while((UART1_FR_R&UART_FR_TXFE) == 0);
  // wait until not busy
  while((UART1_FR_R&UART_FR_BUSY));
}

