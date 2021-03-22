# File: Makefile
# Author: Matthew Yu
# Last Modified: 03/18/21
# Description: A makefile and flash script for TM4C projects. Use at your own
# risk. Developed using Anna Zeng's, Michelle Park's, and Julie Zelenski's page:
# Guide: Using make for ARM Cross-Development.

################################################################################
# USER DEFINED VARIABLES
################################################################################
# The path to the c implementation file with main() to compile and link.
PROJECT_FILE := examples/GPIO/GPIOExample.c
# The anticipated object file dependencies required to link the executable.
PROJECT_DEPENDENCY_OBJS := inc/PLL src/GPIO src/Misc

# The output directory.
BUILD_DIR = build

# Base path of the TM4C123Drivers project.
PATH = /home/matt1500/Github/TM4C123Drivers/

# Used by Make to find where programs like gcc, find, or openocde live.
# We could just wrap it in a shell, but this is easier.
PROGRAM_PATH = /usr/bin/

# The board configuration file used for openOCD.
BOARD = board/ti_ek-tm4c123gxl.cfg

################################################################################
# MAKEFILE VARIABLES
################################################################################

# Extract project name, used as the executable name, from PROJECT_FILE.
PROJECT_NAME := $(notdir $(basename $(PROJECT_FILE)))
# Extract project dependency objects.
PROJECT_DEPENDENCY_OBJS := $(addprefix $(BUILD_DIR)/,$(addsuffix .o, $(PROJECT_DEPENDENCY_OBJS)))

# Our lists for sources. Used for sending object files compiled from
# implementation files to the build directory.
C_LIB_SRCS := $(wildcard lib/*/*.c)
C_LIB_OBJS := ${C_LIB_SRCS:.c=.o}
C_LIB_OBJPATHS := $(addprefix $(BUILD_DIR)/src/,$(notdir $(C_LIB_OBJS)))

# Our lists for includes. Used for sending object files compiled from
# implementation files to the build directory.
C_INC_SRCS := $(wildcard inc/*.c)
C_INC_OBJS := ${C_INC_SRCS:.c=.o}
C_INC_OBJPATHS := $(addprefix $(BUILD_DIR)/inc/,$(notdir $(C_INC_OBJS)))

# Alias to the cross compiler.
CC := $(PROGRAM_PATH)arm-none-eabi-gcc

# Compiler flags.
# -D__TI_COMPILER_VERSION__
#	Triggers some library code dependent on Keil
# 	compiler vs CCS compilers.
# -Wall
#	Gives more compiler warnings. As long as we aren't Rust, we can't have
# 	enough of them.
CFLAGS = -D__TI_COMPILER_VERSION__ -Wall

# Linker flags.
# -lm
# 	Adds the math library dependency. See https://stackoverflow.com/a/12824148.
# --specs=nosys.specs
# 	Required for arm-none-eabi-gcc compiling to prevent undefined references to
# 	__exit. See https://stackoverflow.com/a/23922211.
LFLAGS = -lm --specs=nosys.specs

# On makefile parsing, we build the build folders if they do not yet exist.
DIRS := $(BUILD_DIR) $(BUILD_DIR)/src $(BUILD_DIR)/inc
$(info $(shell mkdir -p $(DIRS)))

# Our general rule for making all of the ingredients for the recipe.
all: $(BUILD_DIR)/$(PROJECT_NAME).bin

# Rule for generating a binary from the elf file.
%.bin: $(BUILD_DIR)/$(PROJECT_NAME).elf
	$(PROGRAM_PATH)arm-none-eabi-objcopy $< -O binary $@

# Rule for generating an elf file from a specific set of object files.
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(C_LIB_OBJPATHS) $(C_INC_OBJPATHS)
	$(CC) $(PROJECT_FILE) $(PROJECT_DEPENDENCY_OBJS) $(LFLAGS) -I$(PATH).. -o $@

# Rule for building files in lib/.
$(BUILD_DIR)/src/%.o: lib/*/%.c
	$(CC) $(CFLAGS) -I$(PATH).. -c $< -o $@

# Rule for building files in inc/.
$(BUILD_DIR)/inc/%.o: inc/%.c
	$(CC) $(CFLAGS) -I$(PATH).. -c $< -o $@



################################################################################
# Other commands.
################################################################################

# Cleans the build directory but leaves intact the folders.
.PHONY: clean
clean: | $(BUILD_DIR)
	$(PROGRAM_PATH)find $(BUILD_DIR) -maxdepth 2 -type f -delete

# Attempt to flash the program to the TM4C using openOCD.
# The command should fail if the bin doesn't exist.
# Flash procedure ripped from our friend Rahul's ninja script
# https://github.com/rrbutani/tm4c-llvm-toolchain/blob/master/common.ninja
# His code lives on in future RASWare, in another form...
# Anyways, this command may need to be executed at least three times in order to
# flash successfully.
# $(PATH)$(BUILD_DIR)/$(PROJECT_NAME).bin
.PHONY: flash
flash: | $(BUILD_DIR) $(BUILD_DIR)/$(PROJECT_NAME).bin
	$(PROGRAM_PATH)openocd \
		-f $(BOARD) \
		-c "init" \
		-c "halt" \
		-c "reset init" \
		-c "sleep 100" \
		-c "flash probe 0" \
		-c "flash write_image erase $(PATH)examples/GPIO/GPIO.bin" \
		-c "sleep 100" \
		-c "verify_image $(PATH)examples/GPIO/GPIO.bin" \
		-c "halt" \
		-c "shutdown"
