# Development Instructions

## Keil

A thorough guide on how to install and use it can be found on Valvano's
[site](http://users.ece.utexas.edu/~valvano/EE445L/).

### Keil Bugfixing and Common Errors

- When trying to flash the program, you might get the error `No ULINK2/ME Device found`. Since we're using the Stellaris ICDI to flash, go to `Options for Target '[Project]'` -> `Debug` tab -> `Use: ULINK2/ME Cortex Debugger`. Scroll to `Stellaris ICDI` option and press OK.
- When compiling the program, errors such as `Lab4.c(14): error:  #5: cannot open source input file "TM4C123Drivers/inc/tm4c123gh6pm.h": No such file or directory` may appear. Go to `Options for Target '[Project]'` -> `C/C++` tab -> `Include Paths` textbox. Include the parent directory path containing `TM4C123Drivers` (i.e., `..;..\..\..;C:\Users\matth\Documents\EE445L`). Press OK after the Compiler control string shows something like the following: `-I C:/Users/matth/Documents/EE445L`.

## Manjaro

### Compiling with arm-none-eabi-gcc

1. Install the arm gcc toolchain crosscompiler:
   `sudo pacman -S arm-none-eabi-gcc`
2. Install relevant libraries:
   `sudo pacman -S arm-none-eabi-newlib`
3. Check where the `arm-none-eabi-gcc` compiler is located:
   `pacman -Ql arm-none-eabi-gcc`
   - In my case, I get a long list with a specific set of binaries found in `/user/bin`

    ```bash
    ...
    arm-none-eabi-gcc /usr/bin/
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-c++
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-cpp
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-g++
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc    # <<< THIS
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc-10.2.0
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc-ar
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc-nm
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc-ranlib
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcov
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcov-dump
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcov-tool
    arm-none-eabi-gcc /usr/bin/arm-none-eabi-lto-dump
    ...
    ```

   - The specific compiler we're looking for, `arm-none-eabi-gcc` is found at `/usr/bin/arm-none-eabi-gcc`. We call this command (or we can alias it in our `.bashrc`) every time we want to compile.
   - Also note we also have a couple other interesting tools that may come in
     handy; namely the c++ compiler, the code coverage tool gcov

4. Jump to the root of the repo and let's try an example compile.

   ```bash
   cd <PATH>/TM4C123Drivers
   /usr/bin/arm-none-eabi-gcc -c examples/Switch/SwitchExample.c -I..
   ```

   Note that the include flag and path `-I..` references the parent folder. This
   is in order to find the <TM4C123Drivers/**> component of the path.

   It should pass at this point an object file, `SwitchExample.o` should be
   found in the TM4C123Drivers folder.

### Flashing with OpenOCD

1. Install OpenOCD:
   `sudo pacman -S openocd`

2. Figure out where build files are:
   Typically, they'll be in a place like `/usr/share/openocd/scripts/board/`
   Since we're using the TM4C, our board is the `ti_ek-tm4c123gxl.cfg`.

3. Try to flash to the specific board:

## Other Linux distributions

The steps should be pretty similar to Manjaro, but using `apt` or another
package manager rather than `pacman`.

