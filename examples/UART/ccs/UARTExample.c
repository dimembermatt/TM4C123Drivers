/**
 * @file UARTExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief An example project showing how to use the UART driver.
 * @version 0.1
 * @date 2021-10-19
 * @copyright Copyright (c) 2021
 * @note
 * Modify __MAIN__ on L14 to determine which main method is executed.
 * __MAIN__ = 0 - Initialization, transmission and capture of UART messages on
 *                loopback mode.
 *            1 - Initialization, transmission of UART messages to a PC via COM.
 */
#define __MAIN__ 1

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/Timer/Timer.h>
#include <lib/UART/UART.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s


#if __MAIN__ == 0
int main(void) {
    /**
     * @brief This program demonstrates in UART mode the sending and receiving
     *        of several variable length messages at 9600 baud rate. Note that
     *        UART0 cannot be used for loopback mode. No hardware configuration
     *        outside of the TM4C Tiva C Launchpad is required for this example.
     */
    
    /* Initialize the internal MCU clock. */
    PLLInit(BUS_80_MHZ);
    
    /* Disable interrupts from occuring during initialization. */
    DisableInterrupts();

    /* Enable systick clocking for delay calls. This doesn't start until 
       EnableInterrupts() is called. */
    DelayInit();

    /* Configure a UART device for communication. */
    UARTConfig_t config = {
        .module=UART_MODULE_1,
        .baudrate=UART_BAUD_9600,
        .dataLength=UART_BITS_8,
        .isFIFODisabled=false,
        .isTwoStopBits=false,
        .parity=UART_PARITY_DISABLED,
        .isLoopback=true
    };
    UART_t uart = UARTInit(config);

    /* Enable interrupts. */
    EnableInterrupts();
    while (1) {
        /* Put a breakpoint at L68. When you run this program in the debugger
           for the first time, the response array (when added to the watch) is
           empty. Run the program again and have it stop at the same breakpoint.
           The response array is now filled with "Hello world!" */
        DelayMillisec(500);

        /* Send "Hello world!" through the UART!" */
        uint8_t size1 = 12;
        uint8_t message1[] = "Hello world!";
        UARTSend(uart, message1, size1);

        /* Capture the response. The response array should now have "Hello 
           world!" */
        uint8_t response[20] = { '\0' };
        UARTReceive(uart, response, 20);
    }
}
#elif __MAIN__ == 1
int main(void) {
    /**
     * @brief This program demonstrates the sending of "Hello World!" to the
     *        user PC at 9600 baudrate. Note that only UART0 on the TM4C is
     *        connected through USB. No other hardware configuration outside
     *        of the TM4C Tiva C Launchpad is required for this example.
     *        A serial monitor application like PuTTy should be open.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    DelayInit();

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

    EnableInterrupts();
    while (1) {
        DelayMillisec(500);

        /* Newline and carriage return special characters are used here so the
           serial output looks nice. */
        uint8_t size1 = 14;
        uint8_t message1[] = "Hello world!\n\r"; 
        UARTSend(uart, message1, size1);
    }
}
#endif
