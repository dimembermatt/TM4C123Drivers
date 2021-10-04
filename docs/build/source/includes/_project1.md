# Creating Your First Project

Alright, congrats on running the RGB program! The next step is to create your
own program and begin to modify it to do what you want.

## Setup

Using CCS, let's create a new project.

![New Project](./images/CCS_new_project.png)

Let's now set it up. For this project, I'll be calling it `rascar` - this was the
theme of the 2017 Robotathon competition!

![Project Setup](./images/CCS_new_project_setup.png)

Note that I also filled in a couple more interesting things. 

- Our target device - the TM4C Launchpad that you'll be programming with, is the
  tm4c123gh6pm chip.
- We'll be using TI's Stellaric ICDI, otherwise known as In-Circuit Debug
  Interface. This is for debugging and flashing the Launchpad.  
- You'll want to create your project folder in the UTRASWare folder - this makes
  access to the libraries and such much easier.
- Finally, you'll want to createan empty project with the main.c file included.
  This contains the `main()` function - the same one in the `RGBExample.c` file
  in the previous tutorial, __Creating Your First Project__.

## Code Structure

> main.c

```c
/**
 * @file main.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Main driver for RASCar - our moving poptart box.
 * @version 1.0
 * @date 2021-09-30
 * @copyright Copyright (c) 2021
 */
int main(void)
{
    return 0;
}
```

Alright, upon creating the new project, we're presented with the following view:

![Empty Project](./images/CCS_new_project_empty.png)

Let's clean up the header and replace it with something more descriptive.

### Adding Includes

> main.c (Revision 1)

```c
/**
 * @file main.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Main driver for RASCar - our moving poptart box.
 * @version 1.0
 * @date 2021-09-30
 * @copyright Copyright (c) 2021
 */

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timer/Timer.h>
#include <raslib/Servo/Servo.h>

int main(void)
{
    return 0;
}
```

Alright, let's include them into our project.

The next step is to figure out what we're going to do. In this project, we'll
define our RASCar as a poptart box with the TM4C and two servo motors - one on
each side of the poptart box. We'll also add indicator lights on the TM4C: one
to show that the "car" is active, and two to show whether the "car" should be
going forwards or backwards.
 
After a bit of searching in our driver libraries, I've identified that we need
the following drivers:

- PLL: To set our system clocks on the Launchpad to 80 MHz (bad things happen if
  we don't do this!)
- GPIO: For turning on our indicator LEDs
- Timer: For creating a heartbeat LED that blinks at 1 Hz
- Servo: For driving our car servo motors

Note that our include calls use angle brackets and a path relative to
`UTRASWare`. We could use quotes, but in this case we did not, for _reasons_. A
dicussion on why you might want to use one or the other can be found on this [SO
post](https://stackoverflow.com/questions/21593/what-is-the-difference-between-include-filename-and-include-filename).
The important part is that the file paths start with `lib`, or `raslib`, which
are direct descendants of UTRASWare's root directory.

Alright, let's compile this!

### Adding the Include Path

![Compiler Error](./images/CCS_new_project_source_inclusion_error.png)

Aww, it's a dud.

This is because we haven't set up the include path to find files at `lib/...`.
If we go to the project properties window and add in the path `../..` (assuming
the project is in `UTRASWare/rascar/.`), our error will now go away and the
project will build successfully.

![Adding Include Path](./images/CCS_new_project_add_include_path.png)

### Adding LEDs

> main.c (Revision 2)

```c
/**
 * @file main.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Main driver for RASCar - our moving poptart box.
 * @version 1.0
 * @date 2021-09-30
 * @copyright Copyright (c) 2021
 */

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timer/Timer.h>
#include <raslib/Servo/Servo.h>


int main(void)
{
    PLLInit(BUS_80_MHZ);

    GPIOConfig_t PF1Config = {
        .pin=PIN_F1,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true
    };
    GPIOConfig_t PF2Config = {
        .pin=PIN_F2,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true
    };
    GPIOConfig_t PF3Config = {
        .pin=PIN_F3,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true
    };


    GPIOInit(PF1Config);
    GPIOInit(PF2Config);
    GPIOInit(PF3Config);

    while(1) {
        GPIOSetBit(PIN_F2, 1);
    }
}
```

The next step to do is to add initializers for the drivers we are using.

This code initializes the system bus clock to 80 MHz. This means that every
atomic operation in the microcontroller runs at 12.5 ns. Reminder that your
computer has a CPU speed of 3.5 GHz!

It also configures three LEDs on pins F1, F2, and F3, corresponding to the Red,
Blue, and Green LEDs respectively. They are set to be internally pulled down,
which means that by default the state of the pin is at 0 V. They are also set to
be outputs, which means that they source current to other devices, up to 8 mA.
Check out GPIO.h for more configuration options, of which there are a lot!

Finally, we set PF2 (blue) to output logic level 1, which is 3.3 V. This should
turn your blue LED on!

Let's try to build the project again.

### Adding Source Files

Drats! It failed again!

![Missing Source Files](./images/CCS_new_project_source_files_missing_error.png)

This is because we don't have the relevant implementation files required in the
project. If we go to the file explorer and link them in, like so:

![Source Inclusion](./images/CCS_new_project_add_source_files.png)

Let's do this for PLL.c, GPIO.c, Timer.c, Servo.c, and FaultHandler.c. The last
file is useful (but not necessary) since it adds internal asserts for some
drivers. When an assert condition is violated during run time, the falls into a
fault handler and turns on the red LED on the TM4C, finally stalling
indefinitely. This is great during testing to make sure your code is configured
properly! It also provides context in the debugger like line numbers, code, etc
that caused the failure.

![Compile Success](./images/CCS_new_project_success_LEDs.png)

Alright. The next thing to do is plug in your TM4C, switch the power switch from
DEVICE to DEBUG (make sure no other power supplies are attached to the VBUS
pin), and press the flash button.

If the blue LED turns on, then we've passed part 1 of this tutorial!

### Giving the TM4C a Heart(beat)

> main.c (Revision 3)

```c
/**
 * @file main.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Main driver for RASCar - our moving poptart box.
 * @version 1.0
 * @date 2021-09-30
 * @copyright Copyright (c) 2021
 */

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timer/Timer.h>
#include <raslib/Servo/Servo.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

void heartbeat(uint32_t * args) {
    GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));
}

int main(void)
{
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    GPIOConfig_t PF1Config = {
        .pin=PIN_F1,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true
    };
    GPIOConfig_t PF2Config = {
        .pin=PIN_F2,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true
    };
    GPIOConfig_t PF3Config = {
        .pin=PIN_F3,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true
    };

    GPIOInit(PF1Config);
    GPIOInit(PF2Config);
    GPIOInit(PF3Config);

    TimerConfig_t heartbeatConfig = {
        .period=freqToPeriod(2, MAX_FREQ),
        .timerTask=heartbeat,
        .isPeriodic=true,
    };
    TimerInit(heartbeatConfig);

    EnableInterrupts();
    while(1) {
        WaitForInterrupt();
    }
}
```

The next step is to give the TM4C a heartbeat to make sure it's operating
nominally. Heartbeats are great to see if your TM4C is stalling at any point in
time.

This code has added a couple new sections to it. 

1) The first of which is a set of declarations for interrupts. These are assembly
   defined declarations found in the `inc/tm4c123gh6pm_startup_ccs.c` file used
   for starting, stopping, and waiting for interrupts.
2) The second of which is a function called `heartbeat`. This is used by a Timer
   interrupt to toggle the red LED at pin F1. More specifically, in the
   `heartbeat` function, we are setting the bit state (0 for off, 1 for on) of
   pin F1 by reading the current state of pin F1 and negating it with the `!`
   symbol.
3) The third of which is a new config called `heartbeatConfig`. In this
   configuration, we defined the period of the interrupt (how long until an
   interrupt occurs) to be every 2 Hz. Why? Because our heartbeat function only
   toggles the LED. If we want the LED to run at 1 Hz, we need to toggle it
   twice! The next step is to assign the `heartbeat` function address to the
   `timerTask` field, and finally assign `true` to `isPeriodic`. Overall, this
   configuration indicates that we will call `heartbeat` at 2 Hz repeatedly
   until stopped.

We then initialize this configuration on the next line, then enable interrupts
and wait for the TM4C to hit the interrupt, until the end of time.

Time to compile!

### Adding Startup Files

This time we get another error:

![Missing Startup Functions](./images/CCS_new_project_startup_files_missing.png)

Despite the fact that TI gives us autogenerated startup files,
`tm4c123gh6pm_startup_ccs.c` and `tm4c123gh6pm.cmd`, these files are incomplete
and are missing a couple things. We have generously provided complete startup
files (`inc/startup.s` for Keil, `inc/tm4c123gh6pm_startup_ccs.c` and
`inc/tm4c123gh6pm.cmd` for CCS). Let's remove the current files and replace them
with our included files.

Rebuilding the project leads to no compiler errors, and flashing it leads to a
red blinking LED!

### Making the rascar Move

> main.c (Revision 4)

```c
/**
 * @file main.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Main driver for RASCar - our moving poptart box.
 * @version 1.0
 * @date 2021-09-30
 * @copyright Copyright (c) 2021
 */

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timer/Timer.h>
#include <raslib/Servo/Servo.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

void heartbeat(uint32_t * args) {
    GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));
}

int main(void)
{
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    GPIOConfig_t PF1Config = {
        .pin=PIN_F1,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true
    };
    GPIOConfig_t PF2Config = {
        .pin=PIN_F2,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true
    };
    GPIOConfig_t PF3Config = {
        .pin=PIN_F3,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true
    };

    GPIOInit(PF1Config);
    GPIOInit(PF2Config);
    GPIOInit(PF3Config);

    TimerConfig_t heartbeatConfig = {
        .period=freqToPeriod(2, MAX_FREQ),
        .timerTask=heartbeat,
        .isPeriodic=true,
    };
    TimerInit(heartbeatConfig);

    ServoConfig_t leftServoConfig = {
        .pin=PIN_B6,
        .timerID=TIMER_1A
    };
    PWM_t leftServo = ServoInit(leftServoConfig);

    ServoConfig_t rightServoConfig = {
        .pin=PIN_B1,
        .timerID=TIMER_2A
    };
    PWM_t rightServo = ServoInit(rightServoConfig);

    ServoSetSpeed(leftServo, 100);
    ServoSetSpeed(rightServo, 100);

    EnableInterrupts();
    while(1) {
        WaitForInterrupt();
    }
}
```

Okay, the final step is to add servo code to the program and get the rascar to
move forward.

We've added two more configurations, a `leftServoConfig` and `rightServoConfig`,
on pin B1 and B6, and timers 1A and 2A. Note that we don't use timer 0A since by
default the heartbeatConfig uses timer 0A.

We then initialize the servo configurations into two structs, `leftServo` and
`rightServo`, and set their speeds to 100 each. This should be the maximum forward
speed for both motors.

Alright, off to the races we go!

...

Okay, I lied, you'll get another error: unresolved symbols for `PWMxxx`. 

Remember what I said earlier about adding source files? Well, we also need to
add all dependency source files! This includes PWM.c, which is used by Servo.h
and Servo.c. Add that in and we'll finish this up.

## Final Note

![Coming Together](./images/coming_together.jpg)

Our rascar base can now blink and move forwards and backwards. This is the first
step in getting a competitive robot for our 2017 competition. The next step
could be a couple of different things, including writing higher level drivers
to make the robot turn, adding sensors to follow the environment, or trying to
optimize the speed, straightness, and precision of movement.

I hope this introductory tutorial on how to use CCS and UTRASWare to get a
functional robot is helpful for you and your team! I look forward to seeing your
robot compete in this year's Robotathon!

~ Matthew Y.