/**
 * GPIO.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for GPIO initialization.
 * Authors: Matthew Yu.
 * Last Modified: 04/05/21
 * 
 * By default, this driver does not support slew rate or drive strength.
 * 
 * Use optional flag `-D__FAST__` or put `#define __FAST__ 1` in your main.c for
 * compiling with GPIOSetBit(Fast) and GPIOGetBit(Fast). Highly recommended for
 * programs using interrupts editing GPIO bits. Tradeoff of 100x speed vs 1kb
 * more lines of DATA. 
 */

/** General imports. */
#include <math.h>
#include <stdlib.h>

/** Device imports. */
#include <inc/RegDefs.h>
#include <inc/tm4c123gh6pm.h>
#include <lib/GPIO/GPIO.h>


/** 
 * This field is either 0, 1, 2, 3, specifying the byte that the interrupt of
 * priority X should be set to.  
 */ 
enum InterruptPriorityIdx {INTA, INTB, INTC, INTD};

/** Struct defining configuration info for each timer. */
struct InterruptSettings {
	/** Priority index of the timer. */
	enum InterruptPriorityIdx priorityIdx;

	/** Address of the relevant priority register. */
	volatile uint32_t * NVIC_PRI_ADDR;

	/** Address of the relevant set enable register. */
	volatile uint32_t * NVIC_EN_ADDR;

	/** Relevant IRQ number of the interrupt. */
	uint32_t IRQ;
};

/** Configuration info for all GPIO ports. */
static struct InterruptSettings interruptSettings[] = {
	{INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI0_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 0},  /* Port A. */
	{INTB, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI0_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 1},  /* Port B. */
	{INTC, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI0_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 2},  /* Port C. */
	{INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI0_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 3},  /* Port D. */
	{INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI1_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 4},  /* Port E. */
	{INTC, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI7_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 30}, /* Port F. */
	/* Port G and H technically exist, but we (the user) never use them directly. */
};

/** Array of potential pin interrupt configurations. */
static GPIOInterruptConfig_t GPIOIntConfig[PIN_COUNT];

/**
 * GPIOInit initializes a GPIO pin given a configuration.
 * 
 * @param pinConfig Configuration for pin.
 * @note By default, this driver does not support slew rate or drive strength.
 */
void GPIOInit(GPIOConfig_t pinConfig) {
	/* Early return on invalid pin_t value. */
	if (pinConfig.GPIOPin >= PIN_COUNT) return;

	/* 1. Activate the clock for the relevant port. */
	GET_REG(SYSCTL_BASE + SYSCTL_RCGCGPIO_OFFSET) |= 
		1 << (pinConfig.GPIOPin / 8); /* 8 pins per port. */

	/* 2. Stall until clock is ready. */
	while ((GET_REG(SYSCTL_BASE + SYSCTL_PRGPIO_OFFSET) & 
		   (1 << (pinConfig.GPIOPin / 8))) == 0) {};

	/** 3. Generate the port offset to find the correct addresses.
	 *	  There are a couple components in this line:
	 * 
	 *	  Note (pin >= PIN_E0). This is a variation of a branchless programming
	 *	  technique. See https://www.youtube.com/watch?v=bVJ-mWWL7cE for more
	 *	  details.
	 * 
	 *	  (((pin - XXX) >> 3) << 12) : 
	 *			Every group of eight pins belongs to the same port, and every
	 *			port has an offset of 0x1000 from each other (typically) (p.685
	 *			in the datasheet). This expression right shifts the pin to mask
	 *			the address (from 0 - 6 | A - F). It then left shifts it to get
	 *			the offset (1 << 12 is 0x1000). This is later appended to
	 *			GPIO_PORT_BASE to get the port address.
	 * 
	 *			The term `- XXX` is a conditional expanded to the following:
	 * 
	 *						  XXX = ((pin >= PIN_E0) << 5)
	 * 
	 *			This conditional is similar to `(pin >= PIN_E0)`. If the pin is 
	 *			in Port E or Port F, we subtract the effective pin by 32 (the 
	 *			enum value of PIN_E0) to get the correct port offset. This is 
	 *			equivalent to 1 << 5, where `(pin >= PIN_E0)` is 1 if true.
	 * 
	 *	  + (pin >= PIN_E0) << 17 : 
	 *			Port E and F are special in that they requires an
	 *			extra offset of 0x0002.0000; we add that base offset, which is 
	 *			equivalent to 1 << 7, where `(pin >= PIN_E0)` is 1 if true.
	 */
	uint32_t portOffset = 
		(((pinConfig.GPIOPin - ((pinConfig.GPIOPin >= PIN_E0) << 5)) >> 3) << 12) + 
		((pinConfig.GPIOPin >= PIN_E0) << 17);

	/* 4. If PC0-3, PD7, PF0, unlock the port. */
	if (pinConfig.GPIOPin == PIN_C0 ||
		pinConfig.GPIOPin == PIN_C1 ||
		pinConfig.GPIOPin == PIN_C2 ||
		pinConfig.GPIOPin == PIN_C3 ||
		pinConfig.GPIOPin == PIN_D7 ||
		pinConfig.GPIOPin == PIN_F0 ) {
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_LOCK_OFFSET) = GPIO_LOCK_KEY;
	}

	/* 5. Determine the pin address. I.E. PF1 (41) % 8 = 1. */
	uint8_t pinAddress = pow(2, pinConfig.GPIOPin % 8);
	
	/* 6. Allow changes to selected pin. */
	GET_REG(GPIO_PORT_BASE + portOffset + GPIO_CR_OFFSET) |= pinAddress;

	/* 7. Set direction of pin. */
	GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DIR_OFFSET) &= ~pinAddress;
	if (pinConfig.isOutput)
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DIR_OFFSET) |= pinAddress;

	/* 8. Set alternative function if required. */
	if (pinConfig.isAlternative) {
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_AFSEL_OFFSET) |= pinAddress;
		uint32_t mask = 0xFFFFFFFF;
		mask &= ~(0xF << ((pinConfig.GPIOPin % 8) * 4));
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_PCTL_OFFSET) &= mask;
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_PCTL_OFFSET) |=
			(pinConfig.alternateFunction << ((pinConfig.GPIOPin % 8) * 4));
	}

	/* 9. Set pin drive strength to 8mA. This step is performed by default. */
	GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DR8R_OFFSET) |= pinAddress;

	/* 10. Set pullup, pulldown, or open drain. */
	if (pinConfig.pull == PULL_UP) {
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_PUR_OFFSET) |= pinAddress;
	} else if (pinConfig.pull == PULL_DOWN) {
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_PDR_OFFSET) |= pinAddress;
	} else if (pinConfig.pull == OPEN_DRAIN) {
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_ODR_OFFSET) |= pinAddress;
	}

	/* 11. Enable as digital or as analog pins. */
	if (pinConfig.isAnalog) {
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_AMSEL_OFFSET) |= pinAddress;
	} else {
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DEN_OFFSET) |= pinAddress;
	}
}

/**
 * GPIOIntInit is a version of the initializer that provides interrupt capability. 
 * 
 * @param pinConfig Configuration for pin.
 * @param pinIntConfig Configuration for pin interrupt.
 * @note Requires the EnableInterrupts() call if edge triggered interrupts are enabled.
 */
void GPIOIntInit(GPIOConfig_t pinConfig, GPIOInterruptConfig_t pinIntConfig) {
	/* Early return on invalid pin_t value. */
	if (pinConfig.GPIOPin >= PIN_COUNT) return;

	GPIOInit(pinConfig);

	/** 1. Generate the port offset to find the correct addresses.
	 *	  There are a couple components in this line:
	 * 
	 *	  Note (pin >= PIN_E0). This is a variation of a branchless programming
	 *	  technique. See https://www.youtube.com/watch?v=bVJ-mWWL7cE for more
	 *	  details.
	 * 
	 *	  (((pin - XXX) >> 3) << 12) : 
	 *			Every group of eight pins belongs to the same port, and every
	 *			port has an offset of 0x1000 from each other (typically) (p.685
	 *			in the datasheet). This expression right shifts the pin to mask
	 *			the address (from 0 - 6 | A - F). It then left shifts it to get
	 *			the offset (1 << 12 is 0x1000). This is later appended to
	 *			GPIO_PORT_BASE to get the port address.
	 * 
	 *			The term `- XXX` is a conditional expanded to the following:
	 * 
	 *						  XXX = ((pin >= PIN_E0) << 5)
	 * 
	 *			This conditional is similar to `(pin >= PIN_E0)`. If the pin is 
	 *			in Port E or Port F, we subtract the effective pin by 32 (the 
	 *			enum value of PIN_E0) to get the correct port offset. This is 
	 *			equivalent to 1 << 5, where `(pin >= PIN_E0)` is 1 if true.
	 * 
	 *	  + (pin >= PIN_E0) << 17 : 
	 *			Port E and F are special in that they requires an
	 *			extra offset of 0x0002.0000; we add that base offset, which is 
	 *			equivalent to 1 << 7, where `(pin >= PIN_E0)` is 1 if true.
	 */
	uint32_t portOffset = 
		(((pinConfig.GPIOPin - ((pinConfig.GPIOPin >= PIN_E0) << 5)) >> 3) << 12) + 
		((pinConfig.GPIOPin >= PIN_E0) << 17);

	/* 2. Determine the pin address. I.E. PF1 (41) % 8 = 1. */
	uint8_t pinAddress = pow(2, pinConfig.GPIOPin % 8);

	/* 3. Enable interrupts if required. */
	if (pinIntConfig.touchTask != NULL || pinIntConfig.releaseTask != NULL) {
		/* 4. Set pin as edge sensitive. */
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IS_OFFSET) &= ~pinAddress;
		if (pinIntConfig.touchTask != NULL && pinIntConfig.releaseTask != NULL) {
			/* 5. Set pin to interrupt on both edges. */
			GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IBE_OFFSET) |= pinAddress;
		} else {
			/* 6. Set pin to interrupt as dictated by GPIOIEV. */
			GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IBE_OFFSET) &= ~pinAddress;
			if (pinIntConfig.touchTask != NULL) {
				/* 7. Set pin to interrupt on a rising edge. */
				GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IEV_OFFSET) |= pinAddress;
			} else {
				/* 8. Set pin to interrupt on a falling edge. */
				GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IEV_OFFSET) &= ~pinAddress;
			}
		}

		/* 9. Clear flag for the pin. */
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_ICR_OFFSET) |= pinAddress;

		/* 10. Arm interrupt for pin. */
		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IM_OFFSET) |= pinAddress;

		/* 11. Set GPIO interrupt priority. */
		uint8_t ID = pinConfig.GPIOPin/8;
		uint32_t mask = 0xFFFFFFFF;
		uint32_t intVal = pinIntConfig.priority << 5;

		/* Our magic number is 8 since to shift a hex value one hex position, we do
		* 4 binary shifts. To do it twice; 8 binary shifts. Our output should
		* something like this for priority 2: 0xFF00FFFF, 0x00400000. */
		mask &= ~(0xFF << (interruptSettings[ID].priorityIdx * 8));
		intVal = intVal << (interruptSettings[ID].priorityIdx * 8);
		(*interruptSettings[ID].NVIC_PRI_ADDR) = 
			((*interruptSettings[ID].NVIC_PRI_ADDR)&mask)|intVal;
		
		/* 12. Enable IRQ X in NVIC. */
		(*interruptSettings[ID].NVIC_EN_ADDR) = 1 << interruptSettings[ID].IRQ;

	}

	/* 13. Assign configuration to the internal data structure. */
	GPIOIntConfig[pinConfig.GPIOPin] = pinIntConfig;
}

/** Internal handler to manage GPIO interrupts. */
#define PORT_E 4
void GPIOGeneric_Handler(pin_t pin) {
	/** 1. Generate the port offset to find the correct addresses.
	 *	  There are a couple components in this line:
	 * 
	 *	  Note (pin >= PIN_E0). This is a variation of a branchless programming
	 *	  technique. See https://www.youtube.com/watch?v=bVJ-mWWL7cE for more
	 *	  details.
	 * 
	 *	  (((pin - XXX) >> 3) << 12) : 
	 *			Every group of eight pins belongs to the same port, and every
	 *			port has an offset of 0x1000 from each other (typically) (p.685
	 *			in the datasheet). This expression right shifts the pin to mask
	 *			the address (from 0 - 6 | A - F). It then left shifts it to get
	 *			the offset (1 << 12 is 0x1000). This is later appended to
	 *			GPIO_PORT_BASE to get the port address.
	 * 
	 *			The term `- XXX` is a conditional expanded to the following:
	 * 
	 *						  XXX = ((pin >= PIN_E0) << 5)
	 * 
	 *			This conditional is similar to `(pin >= PIN_E0)`. If the pin is 
	 *			in Port E or Port F, we subtract the effective pin by 32 (the 
	 *			enum value of PIN_E0) to get the correct port offset. This is 
	 *			equivalent to 1 << 5, where `(pin >= PIN_E0)` is 1 if true.
	 * 
	 *	  + (pin >= PIN_E0) << 17 : 
	 *			Port E and F are special in that they requires an
	 *			extra offset of 0x0002.0000; we add that base offset, which is 
	 *			equivalent to 1 << 7, where `(pin >= PIN_E0)` is 1 if true.
	 */
	uint32_t portOffset = 
		(((pin - ((pin >= PIN_E0) << 5)) >> 3) << 12) + ((pin >= PIN_E0) << 17);

	/* 2. Find which pin triggered an interrupt. Could be multiple at once. */
	for (uint8_t i = 0; i < 8; i++) {
		if (GET_REG(GPIO_PORT_BASE + portOffset + GPIO_MIS_OFFSET) & (0x1 << i)) {
			/* Acknowledge interrupt flag. */
			GET_REG(GPIO_PORT_BASE + portOffset + GPIO_ICR_OFFSET) |= 0x1 << i;
			
			/* Get pin number. */
			pin_t pinIdx = (pin_t)(pin + i);

			if (GPIOIntConfig[pinIdx].touchTask != NULL && GPIOIntConfig[pinIdx].releaseTask != NULL) {
				/* Both edge triggered. */
				/* Get pin status. */
				bool status = GPIOGetBit(pinIdx);
				/* Figure out if rising or falling edge by reading the data register. */
				if (status == 1 && GPIOIntConfig[pinIdx].pinStatus == LOWERED) {
					/* Rising edge. */
					GPIOIntConfig[pinIdx].touchTask();
					GPIOIntConfig[pinIdx].pinStatus = RAISED;
				} else if (status == 0 && GPIOIntConfig[pinIdx].pinStatus == RAISED){
					/* Falling edge. */
					GPIOIntConfig[pinIdx].releaseTask();
					GPIOIntConfig[pinIdx].pinStatus = LOWERED;
				}
			} else if (GPIOIntConfig[pinIdx].touchTask != NULL) {
				/* Rising edge trigger. */
				GPIOIntConfig[pinIdx].touchTask();
				GPIOIntConfig[pinIdx].pinStatus = RAISED;
			} else {
				/* Falling edge trigger. */
				GPIOIntConfig[pinIdx].releaseTask();
				GPIOIntConfig[pinIdx].pinStatus = LOWERED;
			}
		}
	}
}

void GPIOPortA_Handler(void) { GPIOGeneric_Handler(PIN_A0); }

void GPIOPortB_Handler(void) { GPIOGeneric_Handler(PIN_B0); }

void GPIOPortC_Handler(void) { GPIOGeneric_Handler(PIN_C0); }

void GPIOPortD_Handler(void) { GPIOGeneric_Handler(PIN_D0); }

void GPIOPortE_Handler(void) { GPIOGeneric_Handler(PIN_E0); }

void GPIOPortF_Handler(void) { GPIOGeneric_Handler(PIN_F0); }

#ifdef __FAST__
/**
 * GPIOSetBit(Fast) is a time optimized version of GPIOSetBit recommended for use
 * by interrupts.
 * 
 * @param pin Pin to set. Assumes it's an output.
 * @param value 0 (false) or 1 (true) value to set pin to.
 * @return Whether the bit was set properly.
 */
bool GPIOSetBit(pin_t pin, bool val) {
	switch (pin) {
		case PIN_A0:
			GET_REG(0x40004004) = val;
			break;
		case PIN_A1:
			GET_REG(0x40004008) = val << 1;
			break;
		case PIN_A2:
			GET_REG(0x40004010) = val << 2;
			break;
		case PIN_A3:
			GET_REG(0x40004020) = val << 3;
			break;
		case PIN_A4:
			GET_REG(0x40004040) = val << 4;
			break;
		case PIN_A5:
			GET_REG(0x40004080) = val << 5;
			break;
		case PIN_A6:
			GET_REG(0x40004100) = val << 6;
			break;
		case PIN_A7:
			GET_REG(0x40004200) = val << 7;
			break;

		case PIN_B0:
			GET_REG(0x40005004) = val;
			break;
		case PIN_B1:
			GET_REG(0x40005008) = val << 1;
			break;
		case PIN_B2:
			GET_REG(0x40005010) = val << 2;
			break;
		case PIN_B3:
			GET_REG(0x40005020) = val << 3;
			break;
		case PIN_B4:
			GET_REG(0x40005040) = val << 4;
			break;
		case PIN_B5:
			GET_REG(0x40005080) = val << 5;
			break;
		case PIN_B6:
			GET_REG(0x40005100) = val << 6;
			break;
		case PIN_B7:
			GET_REG(0x40005200) = val << 7;
			break;
		
		case PIN_C0:
			GET_REG(0x40006004) = val;
			break;
		case PIN_C1:
			GET_REG(0x40006008) = val << 1;
			break;
		case PIN_C2:
			GET_REG(0x40006010) = val << 2;
			break;
		case PIN_C3:
			GET_REG(0x40006020) = val << 3;
			break;
		case PIN_C4:
			GET_REG(0x40006040) = val << 4;
			break;
		case PIN_C5:
			GET_REG(0x40006080) = val << 5;
			break;
		case PIN_C6:
			GET_REG(0x40006100) = val << 6;
			break;
		case PIN_C7:
			GET_REG(0x40006200) = val << 7;
			break;

		case PIN_D0:
			GET_REG(0x40007004) = val;
			break;
		case PIN_D1:
			GET_REG(0x40007008) = val << 1;
			break;
		case PIN_D2:
			GET_REG(0x40007010) = val << 2;
			break;
		case PIN_D3:
			GET_REG(0x40007020) = val << 3;
			break;
		case PIN_D4:
			GET_REG(0x40007040) = val << 4;
			break;
		case PIN_D5:
			GET_REG(0x40007080) = val << 5;
			break;
		case PIN_D6:
			GET_REG(0x40007100) = val << 6;
			break;
		case PIN_D7:
			GET_REG(0x40007200) = val << 7;
			break;

		case PIN_E0:
			GET_REG(0x40024004) = val;
			break;
		case PIN_E1:
			GET_REG(0x40024008) = val << 1;
			break;
		case PIN_E2:
			GET_REG(0x40024010) = val << 2;
			break;
		case PIN_E3:
			GET_REG(0x40024020) = val << 3;
			break;
		case PIN_E4:
			GET_REG(0x40024040) = val << 4;
			break;
		case PIN_E5:
			GET_REG(0x40024080) = val << 5;
			break;
		case PIN_E6:
			GET_REG(0x40024100) = val << 6;
			break;
		case PIN_E7:
			GET_REG(0x40024200) = val << 7;
			break;

		case PIN_F0:
			GET_REG(0x40025004) = val;
			break;
		case PIN_F1:
			GET_REG(0x40025008) = val << 1;
			break;
		case PIN_F2:
			GET_REG(0x40025010) = val << 2;
			break;
		case PIN_F3:
			GET_REG(0x40025020) = val << 3;
			break;
		case PIN_F4:
			GET_REG(0x40025040) = val << 4;
			break;
		case PIN_F5:
			GET_REG(0x40025080) = val << 5;
			break;
		case PIN_F6:
			GET_REG(0x40025100) = val << 6;
			break;
		case PIN_F7:
			GET_REG(0x40025200) = val << 7;
			break;
		default:
			return false;
	}
	return true;
}

/**
 * GPIOGetBit(Fast) is a time optimized version of GPIOGetBit recommended for use
 * by interrupts.
 * 
 * @param pin Pin to set. Assumes it's an output.
 * @return value 0 (false) or 1 (true) value of the pin.
 */
bool GPIOGetBit(pin_t pin) {
	switch (pin) {
		case PIN_A0:
			return GET_REG(0x40004004);
		case PIN_A1:
			return GET_REG(0x40004008);
		case PIN_A2:
			return GET_REG(0x40004010);
		case PIN_A3:
			return GET_REG(0x40004020);
		case PIN_A4:
			return GET_REG(0x40004040);
		case PIN_A5:
			return GET_REG(0x40004080);
		case PIN_A6:
			return GET_REG(0x40004100);
		case PIN_A7:
			return GET_REG(0x40004200);

		case PIN_B0:
			return GET_REG(0x40005004);
		case PIN_B1:
			return GET_REG(0x40005008);
		case PIN_B2:
			return GET_REG(0x40005010);
		case PIN_B3:
			return GET_REG(0x40005020);
		case PIN_B4:
			return GET_REG(0x40005040);
		case PIN_B5:
			return GET_REG(0x40005080);
		case PIN_B6:
			return GET_REG(0x40005100);
		case PIN_B7:
			return GET_REG(0x40005200);
		
		case PIN_C0:
			return GET_REG(0x40006004);
		case PIN_C1:
			return GET_REG(0x40006008);
		case PIN_C2:
			return GET_REG(0x40006010);
		case PIN_C3:
			return GET_REG(0x40006020);
		case PIN_C4:
			return GET_REG(0x40006040);
		case PIN_C5:
			return GET_REG(0x40006080);
		case PIN_C6:
			return GET_REG(0x40006100);
		case PIN_C7:
			return GET_REG(0x40006200);

		case PIN_D0:
			return GET_REG(0x40007004);
		case PIN_D1:
			return GET_REG(0x40007008);
		case PIN_D2:
			return GET_REG(0x40007010);
		case PIN_D3:
			return GET_REG(0x40007020);
		case PIN_D4:
			return GET_REG(0x40007040);
		case PIN_D5:
			return GET_REG(0x40007080);
		case PIN_D6:
			return GET_REG(0x40007100);
		case PIN_D7:
			return GET_REG(0x40007200);

		case PIN_E0:
			return GET_REG(0x40024004);
		case PIN_E1:
			return GET_REG(0x40024008);
		case PIN_E2:
			return GET_REG(0x40024010);
		case PIN_E3:
			return GET_REG(0x40024020);
		case PIN_E4:
			return GET_REG(0x40024040);
		case PIN_E5:
			return GET_REG(0x40024080);
		case PIN_E6:
			return GET_REG(0x40024100);
		case PIN_E7:
			return GET_REG(0x40024200);

		case PIN_F0:
			return GET_REG(0x40025004);
		case PIN_F1:
			return GET_REG(0x40025008);
		case PIN_F2:
			return GET_REG(0x40025010);
		case PIN_F3:
			return GET_REG(0x40025020);
		case PIN_F4:
			return GET_REG(0x40025040);
		case PIN_F5:
			return GET_REG(0x40025080);
		case PIN_F6:
			return GET_REG(0x40025100);
		case PIN_F7:
			return GET_REG(0x40025200);
		default:
			return false;
	}
}

#else
/**
 * GPIOSetBit sets the bit for an (assumed) output GPIO pin.
 * Substitute for the following:
 * 
 * Option 1:
 *		#define PF1 	  (*((volatile uint32_t *)0x40025008))
 *		PF1 ^= 0x2;
 * 
 * Option 2:
 *		GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DATA_OFFSET) ^= 0x2;
 * 
 * @param pin Pin to set. Assumes it's an output.
 * @param value 0 (false) or 1 (true) value to set pin to.
 * @return Whether the bit was set properly.
 */
bool GPIOSetBit(pin_t pin, bool value) {
	/* Early return on invalid pin_t value. */
	if (pin >= PIN_COUNT) return false;

	/** 1. Generate the port offset to find the correct addresses.
	 *	  There are a couple components in this line:
	 * 
	 *	  Note (pin >= PIN_E0). This is a variation of a branchless programming
	 *	  technique. See https://www.youtube.com/watch?v=bVJ-mWWL7cE for more
	 *	  details.
	 * 
	 *	  (((pin - XXX) >> 3) << 12) : 
	 *			Every group of eight pins belongs to the same port, and every
	 *			port has an offset of 0x1000 from each other (typically) (p.685
	 *			in the datasheet). This expression right shifts the pin to mask
	 *			the address (from 0 - 6 | A - F). It then left shifts it to get
	 *			the offset (1 << 12 is 0x1000). This is later appended to
	 *			GPIO_PORT_BASE to get the port address.
	 * 
	 *			The term `- XXX` is a conditional expanded to the following:
	 * 
	 *						  XXX = ((pin >= PIN_E0) << 5)
	 * 
	 *			This conditional is similar to `(pin >= PIN_E0)`. If the pin is 
	 *			in Port E or Port F, we subtract the effective pin by 32 (the 
	 *			enum value of PIN_E0) to get the correct port offset. This is 
	 *			equivalent to 1 << 5, where `(pin >= PIN_E0)` is 1 if true.
	 * 
	 *	  + (pin >= PIN_E0) << 17 : 
	 *			Port E and F are special in that they requires an
	 *			extra offset of 0x0002.0000; we add that base offset, which is 
	 *			equivalent to 1 << 7, where `(pin >= PIN_E0)` is 1 if true.
	 */
	uint32_t portOffset = 
		(((pin - ((pin >= PIN_E0) << 5)) >> 3) << 12) + ((pin >= PIN_E0) << 17);

	/* 2. Determine the pin address. I.E. PF1 (41) % 8 = 1. */
	pin &= 0x07;
	GET_REG(GPIO_PORT_BASE + portOffset + (1 << (pin + 2))) = value << pin;
	return true;
}

/**
 * GPIOGetBit returns the value of the bit at the specified GPIO pin.
 * Substitute for the following:
 * 
 * Option 1:
 *		#define PF1 	  (*((volatile uint32_t *)0x40025008))
 *		val = PF1 & 0x2;
 * 
 * Option 2:
 *		val = GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DATA_OFFSET) 
 *				&= 0x2;
 * 
 * @param pin Pin to set. Assumes it's an output.
 * @return value 0 (false) or 1 (true) value of the pin.
 */
bool GPIOGetBit(pin_t pin) {
	/* Early return on invalid pin_t value. */
	if (pin >= PIN_COUNT) return 0;

	/** 1. Generate the port offset to find the correct addresses.
	 *	  There are a couple components in this line:
	 * 
	 *	  Note (pin >= PIN_E0). This is a variation of a branchless programming
	 *	  technique. See https://www.youtube.com/watch?v=bVJ-mWWL7cE for more
	 *	  details.
	 * 
	 *	  (((pin - XXX >> 3) << 12) : 
	 *			Every group of eight pins belongs to the same port, and every
	 *			port has an offset of 0x1000 from each other (typically) (p.685
	 *			in the datasheet). This expression right shifts the pin to mask
	 *			the address (from 0 - 6 | A - F). It then left shifts it to get
	 *			the offset (1 << 12 is 0x1000). This is later appended to
	 *			GPIO_PORT_BASE to get the port address.
	 * 
	 *			The term `- XXX` is a conditional expanded to the following:
	 * 
	 *						  XXX = ((pin >= PIN_E0) << 5)
	 * 
	 *			This conditional is similar to `(pin >= PIN_E0)`. If the pin is 
	 *			in Port E or Port F, we subtract the effective pin by 32 (the 
	 *			enum value of PIN_E0) to get the correct port offset. This is 
	 *			equivalent to 1 << 5, where `(pin >= PIN_E0)` is 1 if true.
	 * 
	 *	  + (pin >= PIN_E0) << 17 : 
	 *			Port E and F are special in that they requires an
	 *			extra offset of 0x0002.0000; we add that base offset, which is 
	 *			equivalent to 1 << 7, where `(pin >= PIN_E0)` is 1 if true.
	 */
	uint32_t portOffset = 
		(((pin - ((pin >= PIN_E0) << 5)) >> 3) << 12) + ((pin >= PIN_E0) << 17);

	/* 2. Determine the pin address. I.E. PF1 (41) % 8 = 1. */
	return GET_REG(GPIO_PORT_BASE + portOffset + (1 << ((pin & 0x07) + 2)));
}
#endif

