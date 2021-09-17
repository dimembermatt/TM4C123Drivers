//*****************************************************************************
//
// Startup code for use with TI's Code Composer Studio.
//
// Copyright (c) 2011-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//
// 9/13/21 - Modified by Matthew Yu.
//*****************************************************************************

#include <stdint.h>

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void ResetISR(void);
static void NMI_Handler(void);
static void HardFault_Handler(void);
static void IntDefaultHandler(void);

//*****************************************************************************
//
// External declaration for the reset handler that is to be called when the
// processor is started
//
//*****************************************************************************
extern void _c_int00(void);

//*****************************************************************************
//
// Linker variable that marks the top of the stack.
//
//*****************************************************************************
extern uint32_t __STACK_TOP;

//*****************************************************************************
//
// External declarations for the interrupt handlers used by the application.
//
//*****************************************************************************
// To be added by user

// TODO: should test if this works
void EnableInterrupts(void) {
    __asm("    CPSIE i");
}

void DisableInterrupts(void) {
    __asm("    CPSID i");
}

void WaitForInterrupt(void) {
    __asm("    WFI");
}

/**
 * Interrupt handlers references. Until defined by a STRONG handler in another file,
 * the default action is to go into the default interrupt handler. This simply enters
 * an infinite loop, preserving the system state for examination by a debugger.
 */
void __attribute__((weak)) MemManage_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) BusFault_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) UsageFault_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) SVC_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) DebugMon_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PendSV_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) SysTick_Handler(void) {
    IntDefaultHandler();
}

void __attribute__((weak)) GPIOPortA_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortB_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortC_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortD_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortE_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) UART0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) UART1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) SSI0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) I2C0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PWM0Fault_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PWM0Generator0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PWM0Generator1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PWM0Generator2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Quadrature0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) ADC0Seq0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) ADC0Seq1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) ADC0Seq2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) ADC0Seq3_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WDT_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer0A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer0B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer1A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer1B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer2A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer2B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Comp0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Comp1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Comp2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) SysCtl_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) FlashCtl_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortF_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortG_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortH_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) UART2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) SSI1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer3A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer3B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) I2C1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Quadrature1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) CAN0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) CAN1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Hibernate_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) USB0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PWM0Generator3_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) uDMA_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) uDMA_Error(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) ADC1Seq0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) ADC1Seq1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) ADC1Seq2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) ADC1Seq3_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortJ_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortK_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortL_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) SSI2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) SSI3_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) UART3_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) UART4_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) UART5_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) UART6_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) UART7_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) I2C2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) I2C3_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer4A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer4B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer5A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Timer5B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer0A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer0B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer1A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer1B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer2A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer2B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer3A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer3B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer4A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer4B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer5A_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) WideTimer5B_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) FPU_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) I2C4_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) I2C5_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortM_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortN_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) Quadrature2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortP_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortP1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortP2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortP3_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortP4_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortP5_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortP6_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortP7_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortQ_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortQ1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortQ2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortQ3_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortQ4_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortQ5_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortQ6_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortQ7_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortR_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) GPIOPortS_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PWM1Generator0_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PWM1Generator1_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PWM1Generator2_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PWM1Generator3_Handler(void) {
    IntDefaultHandler();
}
void __attribute__((weak)) PWM1Fault_Handler(void) {
    IntDefaultHandler();
}

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void Reset_Handler(void) {
    //
    // Jump to the CCS C initialization routine.  This will enable the
    // floating-point unit as well, so that does not need to be done here.
    //
    __asm("    .global _c_int00\n"
          "    b.w     _c_int00");
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void NMI_Handler(void) {
    //
    // Enter an infinite loop.
    //
    while(1) {}
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void HardFault_Handler(void) {
    //
    // Enter an infinite loop.
    //
    while(1) {}
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void IntDefaultHandler(void) {
    //
    // Go into an infinite loop.
    //
    while(1) {}
}

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000 or at the start of
// the program if located at a start address other than 0.
//
//*****************************************************************************
#pragma DATA_SECTION(g_pfnVectors, ".intvecs")
void (* const g_pfnVectors[])(void) =
{
    (void (*)(void))((uint32_t)&__STACK_TOP),
                                            // The initial stack pointer
    Reset_Handler,                          // The reset handler
    NMI_Handler,                            // The NMI handler
    HardFault_Handler,                      // The hard fault handler
    MemManage_Handler,                      // The MPU fault handler
    BusFault_Handler,                       // The bus fault handler
    UsageFault_Handler,                     // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    SVC_Handler,                            // SVCall handler
    DebugMon_Handler,                       // Debug monitor handler
    0,                                      // Reserved
    PendSV_Handler,                         // The PendSV handler
    SysTick_Handler,                        // The SysTick handler

    // External Interrupts
    GPIOPortA_Handler,                      // GPIO Port A
    GPIOPortB_Handler,                      // GPIO Port B
    GPIOPortC_Handler,                      // GPIO Port C
    GPIOPortD_Handler,                      // GPIO Port D
    GPIOPortE_Handler,                      // GPIO Port E
    UART0_Handler,                          // UART0 Rx and Tx
    UART1_Handler,                          // UART1 Rx and Tx
    SSI0_Handler,                           // SSI0 Rx and Tx
    I2C0_Handler,                           // I2C0 Master and Slave
    PWM0Fault_Handler,                      // PWM Fault
    PWM0Generator0_Handler,                 // PWM Generator 0
    PWM0Generator1_Handler,                 // PWM Generator 1
    PWM0Generator2_Handler,                 // PWM Generator 2
    Quadrature0_Handler,                    // Quadrature Encoder 0
    ADC0Seq0_Handler,                       // ADC Sequence 0
    ADC0Seq1_Handler,                       // ADC Sequence 1
    ADC0Seq2_Handler,                       // ADC Sequence 2
    ADC0Seq3_Handler,                       // ADC Sequence 3
    WDT_Handler,                            // Watchdog timer
    Timer0A_Handler,                        // Timer 0 subtimer A
    Timer0B_Handler,                        // Timer 0 subtimer B
    Timer1A_Handler,                        // Timer 1 subtimer A
    Timer1B_Handler,                        // Timer 1 subtimer B
    Timer2A_Handler,                        // Timer 2 subtimer A
    Timer2B_Handler,                        // Timer 2 subtimer B
    Comp0_Handler,                          // Analog Comparator 0
    Comp1_Handler,                          // Analog Comparator 1
    Comp2_Handler,                          // Analog Comparator 2
    SysCtl_Handler,                         // System Control (PLL, OSC, BO)
    FlashCtl_Handler,                       // FLASH Control
    GPIOPortF_Handler,                      // GPIO Port F
    GPIOPortG_Handler,                      // GPIO Port G
    GPIOPortH_Handler,                      // GPIO Port H
    UART2_Handler,                          // UART2 Rx and Tx
    SSI1_Handler,                           // SSI1 Rx and Tx
    Timer3A_Handler,                        // Timer 3 subtimer A
    Timer3B_Handler,                        // Timer 3 subtimer B
    I2C1_Handler,                           // I2C1 Master and Slave
    Quadrature1_Handler,                    // Quadrature Encoder 1
    CAN0_Handler,                           // CAN0
    CAN1_Handler,                           // CAN1
    0,                                      // Reserved
    0,                                      // Reserved
    Hibernate_Handler,                      // Hibernate
    USB0_Handler,                           // USB0
    PWM0Generator3_Handler,                 // PWM Generator 3
    uDMA_Handler,                           // uDMA Software Transfer
    uDMA_Error,                             // uDMA Error
    ADC1Seq0_Handler,                       // ADC1 Sequence 0
    ADC1Seq1_Handler,                       // ADC1 Sequence 1
    ADC1Seq2_Handler,                       // ADC1 Sequence 2
    ADC1Seq3_Handler,                       // ADC1 Sequence 3
    0,                                      // Reserved
    0,                                      // Reserved
    GPIOPortJ_Handler,                      // GPIO Port J
    GPIOPortK_Handler,                      // GPIO Port K
    GPIOPortL_Handler,                      // GPIO Port L
    SSI2_Handler,                           // SSI2 Rx and Tx
    SSI3_Handler,                           // SSI3 Rx and Tx
    UART3_Handler,                          // UART3 Rx and Tx
    UART4_Handler,                          // UART4 Rx and Tx
    UART5_Handler,                          // UART5 Rx and Tx
    UART6_Handler,                          // UART6 Rx and Tx
    UART7_Handler,                          // UART7 Rx and Tx
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    I2C2_Handler,                           // I2C2 Master and Slave
    I2C3_Handler,                           // I2C3 Master and Slave
    Timer4A_Handler,                        // Timer 4 subtimer A
    Timer4B_Handler,                        // Timer 4 subtimer B
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    Timer5A_Handler,                        // Timer 5 subtimer A
    Timer5B_Handler,                        // Timer 5 subtimer B
    WideTimer0A_Handler,                    // Wide Timer 0 subtimer A
    WideTimer0B_Handler,                    // Wide Timer 0 subtimer B
    WideTimer1A_Handler,                    // Wide Timer 1 subtimer A
    WideTimer1B_Handler,                    // Wide Timer 1 subtimer B
    WideTimer2A_Handler,                    // Wide Timer 2 subtimer A
    WideTimer2B_Handler,                    // Wide Timer 2 subtimer B
    WideTimer3A_Handler,                    // Wide Timer 3 subtimer A
    WideTimer3B_Handler,                    // Wide Timer 3 subtimer B
    WideTimer4A_Handler,                    // Wide Timer 4 subtimer A
    WideTimer4B_Handler,                    // Wide Timer 4 subtimer B
    WideTimer5A_Handler,                    // Wide Timer 5 subtimer A
    WideTimer5B_Handler,                    // Wide Timer 5 subtimer B
    FPU_Handler,                            // FPU
    0,                                      // Reserved
    0,                                      // Reserved
    I2C4_Handler,                           // I2C4 Master and Slave
    I2C5_Handler,                           // I2C5 Master and Slave
    GPIOPortM_Handler,                      // GPIO Port M
    GPIOPortN_Handler,                      // GPIO Port N
    Quadrature2_Handler,                    // Quadrature Encoder 2
    0,                                      // Reserved
    0,                                      // Reserved
    GPIOPortP_Handler,                      // GPIO Port P (Summary or P0)
    GPIOPortP1_Handler,                     // GPIO Port P1
    GPIOPortP2_Handler,                     // GPIO Port P2
    GPIOPortP3_Handler,                     // GPIO Port P3
    GPIOPortP4_Handler,                     // GPIO Port P4
    GPIOPortP5_Handler,                     // GPIO Port P5
    GPIOPortP6_Handler,                     // GPIO Port P6
    GPIOPortP7_Handler,                     // GPIO Port P7
    GPIOPortQ_Handler,                      // GPIO Port Q (Summary or Q0)
    GPIOPortQ1_Handler,                     // GPIO Port Q1
    GPIOPortQ2_Handler,                     // GPIO Port Q2
    GPIOPortQ3_Handler,                     // GPIO Port Q3
    GPIOPortQ4_Handler,                     // GPIO Port Q4
    GPIOPortQ5_Handler,                     // GPIO Port Q5
    GPIOPortQ6_Handler,                     // GPIO Port Q6
    GPIOPortQ7_Handler,                     // GPIO Port Q7
    GPIOPortR_Handler,                      // GPIO Port R
    GPIOPortS_Handler,                      // GPIO Port S
    PWM1Generator0_Handler,                 // PWM 1 Generator 0
    PWM1Generator1_Handler,                 // PWM 1 Generator 1
    PWM1Generator2_Handler,                 // PWM 1 Generator 2
    PWM1Generator3_Handler,                 // PWM 1 Generator 3
    PWM1Fault_Handler                       // PWM 1 Fault
};
