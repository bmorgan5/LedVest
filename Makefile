#************************************************************************
# Makefile for led_vest project using Teensy3.2
# Benjamin Morgan, 2016
# github.com/bmorgan5/led_vest
#************************************************************************

# TODO: Do I need these here?
# set your MCU type here, or make command line `make MCU=MK20DX256`
# MCU=MK20DX256

# # make it lower case
# LOWER_MCU := $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$(MCU)))))))))))))))))))))))))))
# MCU_LD = $(LOWER_MCU).ld

# # configurable options
# OPTIONS = -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DUSING_MAKEFILE

# # options needed by many Arduino libraries to configure for Teensy 3.0
# OPTIONS += -D__$(MCU)__ -DARDUINO=10600 -DTEENSYDUINO=121

#end TODO


# Other Makefiles and project templates for Teensy 3.x:
#
# https://github.com/apmorton/teensy-template
# https://github.com/xxxajk/Arduino_Makefile_master
# https://github.com/JonHylands/uCee

# The name of your project (used to name the compiled .hex file)
TARGET = $(notdir $(CURDIR))

# directory to build in
BUILDDIR = $(abspath $(CURDIR)/build)

# teensy version 3.2
TEENSY = 32

# Set to 24000000, 48000000, or 96000000 to set CPU core speed
TEENSY_CORE_SPEED = 48000000 

#configurable options
OPTIONS = -DUSB_SERIAL -DLAYOUT_US_ENGLISH

#************************************************************************
# Location of Teensyduino utilities, Toolchain, and Arduino Libraries.
# To use this makefile without Arduino, copy the resources from these
# locations and edit the pathnames.  The rest of Arduino is not needed.
#************************************************************************

TOOLSPATH = $(CURDIR)/tools

COREPATH = teensy3

LIBRARYPATH = libraries

COMPILERPATH = $(TOOLSPATH)/arm/bin

#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -g -Os -mthumb -ffunction-sections -fdata-sections -nostdlib -MMD $(OPTIONS) -DTEENSYDUINO=124 -DF_CPU=$(TEENSY_CORE_SPEED) -Isrc -I$(COREPATH)
CPPFLAGS += -D__MK20DX256__ -mcpu=cortex-m4

# CPPFLAGS = compiler options for C and C++
# CPPFLAGS = -Wall -g -Os -mcpu=cortex-m4 -mthumb -MMD $(OPTIONS) -I.

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -Os -Wl,--gc-sections -mthumb

LDSCRIPT = $(COREPATH)/mk20dx256.ld

LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)


# linker options
# LDFLAGS = -Os -Wl,--gc-sections,--defsym=__rtc_localtime=0 --specs=nano.specs -mcpu=cortex-m4 -mthumb -T$(MCU_LD)

# additional libraries to link
LIBS = -lm

# set arduino define if given
ifdef ARDUINO
	CPPFLAGS += -DARDUINO=$(ARDUINO)
else
	CPPFLAGS += -DUSING_MAKEFILE
endif

# names for the compiler programs
CC = $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
CXX = $(abspath $(COMPILERPATH))/arm-none-eabi-g++
OBJCOPY = $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
SIZE = $(abspath $(COMPILERPATH))/arm-none-eabi-size

# automatically create lists of the sources and objects
# TODO: this does not handle Arduino libraries yet...
LC_FILES := $(wildcard $(LIBRARYPATH)/*/*.c)
LCPP_FILES := $(wildcard $(LIBRARYPATH)/*/*.cpp)
TC_FILES := $(wildcard $(COREPATH)/*.c)
TCPP_FILES := $(wildcard $(COREPATH)/*.cpp)
C_FILES := $(wildcard src/*.c)
CPP_FILES := $(wildcard src/*.cpp)
INO_FILES := $(wildcard src/*.ino)

#include paths for libraries
L_INC := $(foreach lib,$(filter %/, $(wildcard $(LIBRARYPATH)/*/)), -I$(lib))

SOURCES := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(INO_FILES:.ino=.o) $(TC_FILES:.c=.o) $(TCPP_FILES:.cpp=.o) $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))

# the actual makefile rules (all .o files built by GNU make's default implicit rules)

all: hex

build: $(TARGET).elf

hex: $(TARGET).hex

post_compile: $(TARGET).hex
	@$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(basename $<)" -path=$(CURDIR) -tools="$(abspath $(TOOLSPATH))"

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot

upload: post_compile reboot

$(BUILDDIR)/%.o: %.c
	@echo "[CC]	$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.cpp
	@echo "[CXX]	$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.ino
	@echo "[CXX]	$<"
	@mkdir -p "$(dir $@)"

$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	@echo "[LD]	$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)


%.hex: %.elf
	@echo "[HEX]	$@"
	@$(SIZE) "$<"
	@$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"


# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	@echo 'Cleaning...'
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(TARGET).elf" "$(TARGET).hex"
