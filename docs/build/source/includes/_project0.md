# Running A Demo Project

Let's start off with a fun example project to flash with to the TM4C. We'll be
flashing the RGB project in UTRASWare/rasexamples/RGB with CCS.

## Setup

1) Import projects from UTRASWare.

![Import Projects](./images/CCS_import_projects.png)

2) Select the directory that has your UTRASWare project.

![Select Directory](./images/CCS_select_directory.png)

3) Select the specific project you want to import (or select all of them, you'll
   need them later on).

![Select Projects](./images/CCS_select_projects.png)

4) Click finish! The projects will load on the sidebar.

![Load Projects](./images/CCS_load_projects.png)

A couple things to notice here:

- *The project viewer on the left side:* this shows your projects in your
  workspace, and within each project are all the files needed to build and run
  your project. These project are typically .c files, or implementation files.
  The build system uses File > Properties > Build > Include Options to search
  for the relevant header files which defines functions and links them together
  into a single binary.
- *The file viewer on the right side:* This shows the code inside of each of your
  files. You write your code here!
- *Build/Compile button.* Whenever you want to test your code, the first thing to
  do is compile it, and check for any errors!
- Errors will show up in the *Compiler and debug view* on the bottom. It'll show
  syntax errors or warnings, and so forth.
- Once your project compiles successfully, you can press either the *Flash or
  Debug button.* The former directly writes the compiled program to your TM4C,
  and the other writes the compiled program, then enters it so you can step
  through the program execution.

5) After building the program, here is what a successful build output should
   look like. A `*.out` file is generated in a local Binaries folder, and this
   is the machine code that the TM4C reads.

![Build project](./images/CCS_build_project.png)

6) Now we press the flash button... And then your TM4C should now spurt out
   rainbow colors at the LEDs next to your reset switch!

## Code Structure

> RGBExample.c

```c
/**
 * (1)
 * @file RGBExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief TM4C goes wild with RGB lighting.
 * @version 0.1
 * @date 2021-09-27
 * @copyright Copyright (c) 2021
 * @note Warning to users who have epilepsy - bright flashing colors.
 */

/** (2) General imports. */
#include <stdlib.h>

/** (3) Device specific imports. */
#include <lib/PLL/PLL.h>
#include <raslib/RGB/RGB.h>

/* (4) */
void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

/* (5) */
int main(void) {
    /* (6) */
    PLLInit(BUS_80_MHZ);

    /* (7) */
    DisableInterrupts();

    /* (8) Initialize the RGB LEDs! */
    RGBInit();

    /* (9) */
    EnableInterrupts();

    /* (10) */
    while(1) {
        /* (11) */
        WaitForInterrupt();
    }
}
```

Alright, now that you've flashed your baby's first TM4C code and have gotten it
(supposedly) to work, let's actually dig into _what_ your program does to make
it change colors on the LEDs.

Our main code consists of the following:

1. This is the documentation header for your file. Here, you should include
   important things like "_what does this file do?_", "_what does this project
   do?_", "_are there any preconditions to use this project?_", "_who should be
   contacted if my TM4C is on fire trying to run this project?_".
2. General imports generally include files such as `stdlib.h`, `stdint.h`
   `stdbool.h` and so on. They are useful for providing abstractions like `NULL`
   and `bool` for use in the program.
3. Device specific imports include the specific driver files required to run the
   main program. Every `.c` or `.h` file may include device dependencies (e.g.
   `Switch.h` depends on `GPIO.h`). These must be defined or otherwise your
   compiler will throw an error that some symbols (e.g. PLLInit) are undefined,
   which means that it cannot find them.
4. These function declarations are defined in the `startup.s` /
   `tm4c123gh6pm_startup_ccs.c` assembly file for managing interrupts.
5. Our main routine. This is the entry point that all programs must have in
   C/C++. 
6. `PLLInit` starts up our internal clock. The TM4C can run at a maximum of 80
   MHZ. Looking into the header file (`PLL.h`) itself, we can see definitions
   for many types of clock frequencies.     
7. Peripherals including timers and switches and so forth can generate
   interrupts. Turn them off during initialization so interrupts do not cause
   unintended effects in initialization routines.
8. This is our initializer for an RGB lighting struct. Looking at
   `raslib/RGB/RGB.h`, we can see from the header declaration documentation that
   the RGB struct uses pins PF1, PF2, PF3, as well as TIMER_5A to run interrupts.
9. Enable the interrupts. This allows the RGB struct to begin stepping through
   color states.
10. Our infinite loop to keep the device from shutting down.
11. This function simply does nothing until an interrupt is triggered.

## Final Note

As a final note, there are many examples presented in `UTRASWare/examples` and
`UTRASWare/rasexamples` that you can import as projects, both in Keil and in CCS
for you to experiment, test, and play with. They utilise some of the essential
APIs required for you to put together a functioning robot for competition, so it
is __HIGHLY__ recommended to visit them! 

Further API documentation for each driver can be found on this site, explaining
their inputs, preconditions, and so forth.