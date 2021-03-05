/**
 * Timers.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for using onboard timers.
 * Authors: Matthew Yu.
 * Last Modified: 03/04/21
 **/

/** Device specific imports. */
#include "Timers.h"


#define TIMERXA_ICR_TATOCINT 0x00000001
#define TIMERXB_ICR_TATOCINT 0x00000100

/** Configuration list of handlers used by timers. */
struct TimerHandlerTasks {
    /** Function pointers called by normal timers. */
    void (*timer0ATask)(void);
    void (*timer0BTask)(void);

    void (*timer1ATask)(void);
    void (*timer1BTask)(void);

    void (*timer2ATask)(void);
    void (*timer2BTask)(void);

    void (*timer3ATask)(void);
    void (*timer3BTask)(void);

    void (*timer4ATask)(void);
    void (*timer4BTask)(void);

    void (*timer5ATask)(void);
    void (*timer5BTask)(void);

    /** Function pointers called by wide timers. */
    void (*timerWide0ATask)(void);
    void (*timerWide0BTask)(void);

    void (*timerWide1ATask)(void);
    void (*timerWide1BTask)(void);

    void (*timerWide2ATask)(void);
    void (*timerWide2BTask)(void);

    void (*timerWide3ATask)(void);
    void (*timerWide3BTask)(void);

    void (*timerWide4ATask)(void);
    void (*timerWide4BTask)(void);

    void (*timerWide5ATask)(void);
    void (*timerWide5BTask)(void);
};

static struct TimerHandlerTasks timerConfig = { NULL };

/**
 * TimerInit initializes an arbitrary timer with a handler function reference.
 * @param timer Enum identifying which timer to initialize.
 * @param period Reload time, in cycles.
 * @param handlerTask Function pointer to what should be called by the TimerXX_Handler.
 * @note Potentially add the following parameters:
 *          - clock mode (i.e. 32-bit vs 16-bit config with CFG_R).
 *          - one shot vs periodic timer mode (TAMR, TBMR).
 *          - count down vs count up (TACDIR inside TAMR, TBCDIR inside TBMR).
 *          - Timer priority (NVIC).
 *       Requires the EnableInterrupts() call if edge triggered interrupts are enabled.
 *       By default the timer is priority 2, below SysTick.
 */
void TimerInit(timer_t timer, uint32_t period, void (*handlerTask)(void)) {
    switch (timer) {
        case TIMER_0A:
            timerConfig.timer0ATask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x01;             // 1) Activate the timer.
            TIMER0_CTL_R &= 0x11111100;             // 2) Disable timerA during setup.
            TIMER0_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER0_TAMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER0_TAILR_R = period - 1;            // 5) Set reload value.
            TIMER0_TAPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER0_ICR_R |= TIMERXA_ICR_TATOCINT;   // 7) Clear timerA timeout flag.
            TIMER0_IMR_R |= 0x00000001;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI4, interrupt[4n+3]).
            NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; 
            NVIC_EN0_R |= 1<<19;                    // 10) Enable IRQ 19 in NVIC.

            TIMER0_CTL_R |= 0x00000001;             // 12) Enable timerA after setup.
            break;
        case TIMER_0B:
            timerConfig.timer0BTask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x01;             // 1) Activate the timer.
            TIMER0_CTL_R &= 0x11110011;             // 2) Disable timerB during setup.
            TIMER0_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER0_TBMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER0_TBILR_R = period - 1;            // 5) Set reload value.
            TIMER0_TBPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER0_ICR_R |= TIMERXB_ICR_TATOCINT;   // 7) Clear timerB timeout flag.
            TIMER0_IMR_R |= 0x00000100;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI5, interrupt[4n]).
            NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFFFF00)|0x00000040;
            NVIC_EN0_R |= 1<<20;                    // 10) Enable IRQ 20 in NVIC.

            TIMER0_CTL_R |= 0x00000100;             // 12) Enable timerB after setup.
            break;
        case TIMER_1A:
            timerConfig.timer1ATask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x02;             // 1) Activate the timer.
            TIMER1_CTL_R &= 0x11111100;             // 2) Disable timerA during setup.
            TIMER1_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER1_TAMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER1_TAILR_R = period - 1;            // 5) Set reload value.
            TIMER1_TAPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER1_ICR_R |= TIMERXA_ICR_TATOCINT;   // 7) Clear timerA timeout flag.
            TIMER1_IMR_R |= 0x00000001;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI5, interrupt[4n+1]).
            NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00004000; 
            NVIC_EN0_R |= 1<<21;                    // 10) Enable IRQ 21 in NVIC.

            TIMER1_CTL_R |= 0x00000001;             // 12) Enable timerA after setup.
            break;
        case TIMER_1B:
            timerConfig.timer1BTask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x02;             // 1) Activate the timer.
            TIMER0_CTL_R &= 0x11110011;             // 2) Disable timerB during setup.
            TIMER0_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER0_TBMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER0_TBILR_R = period - 1;            // 5) Set reload value.
            TIMER0_TBPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER0_ICR_R |= TIMERXB_ICR_TATOCINT;   // 7) Clear timerB timeout flag.
            TIMER0_IMR_R |= 0x00000100;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI5, interrupt[4n+2]).
            NVIC_PRI5_R = (NVIC_PRI5_R&0xFF00FFFF)|0x00400000;
            NVIC_EN0_R |= 1<<22;                    // 10) Enable IRQ 22 in NVIC.

            TIMER0_CTL_R |= 0x00000100;             // 12) Enable timerB after setup.
            break;
        case TIMER_2A:
            timerConfig.timer2ATask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x04;             // 1) Activate the timer.
            TIMER2_CTL_R &= 0x11111100;             // 2) Disable timerA during setup.
            TIMER2_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER2_TAMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER2_TAILR_R = period - 1;            // 5) Set reload value.
            TIMER2_TAPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER2_ICR_R |= TIMERXA_ICR_TATOCINT;   // 7) Clear timerA timeout flag.
            TIMER2_IMR_R |= 0x00000001;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI5, interrupt[4n+3]).
            NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x40000000; 
            NVIC_EN0_R |= 1<<23;                    // 10) Enable IRQ 23 in NVIC.

            TIMER2_CTL_R |= 0x00000001;             // 12) Enable timerA after setup.
            break;
        case TIMER_2B:
            timerConfig.timer2BTask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x04;             // 1) Activate the timer.
            TIMER2_CTL_R &= 0x11110011;             // 2) Disable timerB during setup.
            TIMER2_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER2_TBMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER2_TBILR_R = period - 1;            // 5) Set reload value.
            TIMER2_TBPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER2_ICR_R |= TIMERXB_ICR_TATOCINT;   // 7) Clear timerB timeout flag.
            TIMER2_IMR_R |= 0x00000100;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI6, interrupt[4n]).
            NVIC_PRI6_R = (NVIC_PRI6_R&0xFFFFFF00)|0x00000040;
            NVIC_EN0_R |= 1<<24;                    // 10) Enable IRQ 24 in NVIC.

            TIMER2_CTL_R |= 0x00000100;             // 12) Enable timerB after setup.
            break;
        case TIMER_3A:
            timerConfig.timer3ATask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x08;             // 1) Activate the timer.
            TIMER3_CTL_R &= 0x11111100;             // 2) Disable timerA during setup.
            TIMER3_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER3_TAMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER3_TAILR_R = period - 1;            // 5) Set reload value.
            TIMER3_TAPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER3_ICR_R |= TIMERXA_ICR_TATOCINT;   // 7) Clear timerA timeout flag.
            TIMER3_IMR_R |= 0x00000001;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI8, interrupt[4n+3]).
            NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x40000000; 
            NVIC_EN1_R |= 1<<(35-32);               // 10) Enable IRQ 35 in NVIC.

            TIMER3_CTL_R |= 0x00000001;             // 12) Enable timerA after setup.
            break;
        case TIMER_3B:
            timerConfig.timer3BTask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x08;             // 1) Activate the timer.
            TIMER3_CTL_R &= 0x11110011;             // 2) Disable timerB during setup.
            TIMER3_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER3_TBMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER3_TBILR_R = period - 1;            // 5) Set reload value.
            TIMER3_TBPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER3_ICR_R |= TIMERXB_ICR_TATOCINT;   // 7) Clear timerB timeout flag.
            TIMER3_IMR_R |= 0x00000100;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI9, interrupt[4n]).
            NVIC_PRI9_R = (NVIC_PRI9_R&0xFFFFFF00)|0x00000040;
            NVIC_EN1_R |= 1<<(36-32);               // 10) Enable IRQ 36 in NVIC.

            TIMER3_CTL_R |= 0x00000100;             // 12) Enable timerB after setup.
            break;
        case TIMER_4A:
            timerConfig.timer4ATask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x10;             // 1) Activate the timer.
            TIMER4_CTL_R &= 0x11111100;             // 2) Disable timerA during setup.
            TIMER4_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER4_TAMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER4_TAILR_R = period - 1;            // 5) Set reload value.
            TIMER4_TAPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER4_ICR_R |= TIMERXA_ICR_TATOCINT;   // 7) Clear timerA timeout flag.
            TIMER4_IMR_R |= 0x00000001;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI17, interrupt[4n+2]).
            NVIC_PRI17_R = (NVIC_PRI17_R&0xFF00FFFF)|0x00400000; 
            NVIC_EN2_R |= 1<<(70-64);               // 10) Enable IRQ 70 in NVIC.

            TIMER4_CTL_R |= 0x00000001;             // 12) Enable timerA after setup.
            break;
        case TIMER_4B:
            timerConfig.timer4BTask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x10;             // 1) Activate the timer.
            TIMER4_CTL_R &= 0x11110011;             // 2) Disable timerB during setup.
            TIMER4_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER4_TBMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER4_TBILR_R = period - 1;            // 5) Set reload value.
            TIMER4_TBPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER4_ICR_R |= TIMERXB_ICR_TATOCINT;   // 7) Clear timerB timeout flag.
            TIMER4_IMR_R |= 0x00000100;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI17, interrupt[4n+3]).
            NVIC_PRI17_R = (NVIC_PRI17_R&0x00FFFFFF)|0x40000000;
            NVIC_EN2_R |= 1<<(71-64);               // 10) Enable IRQ 71 in NVIC.

            TIMER4_CTL_R |= 0x00000100;             // 12) Enable timerB after setup.
            break;
        case TIMER_5A:
            timerConfig.timer5ATask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x20;             // 1) Activate the timer.
            TIMER5_CTL_R &= 0x11111100;             // 2) Disable timerA during setup.
            TIMER5_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER5_TAMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER5_TAILR_R = period - 1;            // 5) Set reload value.
            TIMER5_TAPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER5_ICR_R |= TIMERXA_ICR_TATOCINT;   // 7) Clear timerA timeout flag.
            TIMER5_IMR_R |= 0x00000001;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI23, interrupt[4n]).
            NVIC_PRI23_R = (NVIC_PRI23_R&0xFFFFFF00)|0x00000040; 
            NVIC_EN2_R |= 1<<(92-64);               // 10) Enable IRQ 92 in NVIC.

            TIMER5_CTL_R |= 0x00000001;             // 12) Enable timerA after setup.
            break;
        case TIMER_5B:
            timerConfig.timer5BTask = handlerTask;  
            SYSCTL_RCGCTIMER_R |= 0x20;             // 1) Activate the timer.
            TIMER5_CTL_R &= 0x11110011;             // 2) Disable timerB during setup.
            TIMER5_CFG_R = 0;                       // 3) Configure for 32-bit mode.
            TIMER5_TBMR_R = 0x2;                    // 4) Configure for periodic mode, count down.
            TIMER5_TBILR_R = period - 1;            // 5) Set reload value.
            TIMER5_TBPR_R = 0;                      // 6) Set prescaler to 1.
            TIMER5_ICR_R |= TIMERXB_ICR_TATOCINT;   // 7) Clear timerB timeout flag.
            TIMER5_IMR_R |= 0x00000100;             // 8) Arm timeoutA interrupt.

            // 9) Set timerA to interrupt priority 2 (PRI23, interrupt[4n+1]).
            NVIC_PRI23_R = (NVIC_PRI23_R&0xFFFF00FF)|0x00004000;
            NVIC_EN2_R |= 1<<(93-64);               // 10) Enable IRQ 93 in NVIC.

            TIMER5_CTL_R |= 0x00000100;             // 12) Enable timerB after setup.
            break;
        default:
            break;
    }
}

void Timer0A_Handler(void) {
    TIMER0_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (timerConfig.timer0ATask != NULL) {
        timerConfig.timer0ATask();
    }
}

void Timer0B_Handler(void) {
    TIMER0_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (timerConfig.timer0BTask != NULL) {
        timerConfig.timer0BTask();
    }
}

void Timer1A_Handler(void) {
    TIMER1_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (timerConfig.timer1ATask != NULL) {
        timerConfig.timer1ATask();
    }
}

void Timer1B_Handler(void) {
    TIMER1_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (timerConfig.timer1BTask != NULL) {
        timerConfig.timer1BTask();
    }
}

void Timer2A_Handler(void) {
    TIMER2_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (timerConfig.timer2ATask != NULL) {
        timerConfig.timer2ATask();
    }
}

void Timer2B_Handler(void) {
    TIMER2_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (timerConfig.timer2BTask != NULL) {
        timerConfig.timer2BTask();
    }
}

void Timer3A_Handler(void) {
    TIMER3_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (timerConfig.timer3ATask != NULL) {
        timerConfig.timer3ATask();
    }
}

void Timer3B_Handler(void) {
    TIMER3_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (timerConfig.timer3BTask != NULL) {
        timerConfig.timer3BTask();
    }
}

void Timer4A_Handler(void) {
    TIMER4_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (timerConfig.timer4ATask != NULL) {
        timerConfig.timer4ATask();
    }
}

void Timer4B_Handler(void) {
    TIMER4_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (timerConfig.timer4BTask != NULL) {
        timerConfig.timer4BTask();
    }
}

/**
void Timer5A_Handler(void) {
    TIMER5_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (timerConfig.timer5ATask != NULL) {
        timerConfig.timer5ATask();
    }
}
*/

void Timer5B_Handler(void) {
    TIMER5_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (timerConfig.timer5BTask != NULL) {
        timerConfig.timer5BTask();
    }
}

void WideTimer0A_Handler(void) {}
void WideTimer0B_Handler(void) {}
void WideTimer1A_Handler(void) {}
void WideTimer1B_Handler(void) {}
void WideTimer2A_Handler(void) {}
void WideTimer2B_Handler(void) {}
void WideTimer3A_Handler(void) {}
void WideTimer3B_Handler(void) {}
void WideTimer4A_Handler(void) {}
void WideTimer4B_Handler(void) {}
void WideTimer5A_Handler(void) {}
void WideTimer5B_Handler(void) {}

/**
 * freqToPeriod converts a desired frequency into the equivalent period in
 * cycles given the base system clock.
 * @param freq Desired frequency.
 * @param maxFreq Base clock frequency. If freq > maxFreq, period = 1 (saturates
 *                at max frequency). Rounds up if maxFreq is not easily
 *                divisible by freq.
 * @return Output period, in cycles.
 */ 
uint32_t freqToPeriod(uint32_t freq, uint32_t maxFreq) {
    return (uint32_t) ceil(maxFreq/freq);	
}
