# Making your own program

## Keil
1. Copy the files `Example.c` and `Example.uvprojx`; these contain the 'starter' code.
2. Rename the files to your desired program name. We assume in this tutorial that you named it `Hello.c` and `Hello.uvprojx`.
3. Open up the project file (i.e. click on `Hello.uvprojx` in a file explorer or open it up from `Keil -> Project -> Open Project`).
4. Open the `Options for Target '<PROGRAM_NAME>'`.
    1. Go to tab `Output`, and change the name of the executable.
    2. Go to tab `Debug`, and click on the Debugger dropdown on the right side of the window. Select `Stellaris ICDI`, assuming you've [downloaded](https://www.ti.com/tool/STELLARIS_ICDI_DRIVERS) and set that up.
    3. Press OK.
5. Go to the Project Window and add/remove implementation files necessary to run your project.
6. Open up `Hello.c` in the Keil IDE and go through the example program to see where you would like to put stuff.
7. Have fun programming!
8. Press the compile buttons on the top toolbar (Tooltip `Build` and `Rebuild`) to compile and observe compilation errors in the Build Output.
9. Flash the program using the Download button on the top toolbar (Tooltip `Download`). Press the RESET button on the TM4C to have it start working.

## Code Composer Studio
TODO: #1
