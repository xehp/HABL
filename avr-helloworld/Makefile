# makefile, written by guido socher
# downloaded from http://linuxfocus.org/English/November2004/article352.shtml
# modified 2005-02-16 by Henrik Bj�rkman to fit my application

# Change here if you have another atmega microcontroller (MCU)
#MCU=atmega88
#MCU=atmega8
#MCU=atmega168
MCU=atmega328p


# List all application header files here (avr files are listed further down):
APPLICATION_HEADER_FILES=version.h

# List all needed application object files here (avr files are listed further down):
APPLICATION_OBJECT_FILES=main.o




# Some generic macros, which compiler and linker to use etc.
CC=avr-gcc
OBJCOPY=avr-objcopy
# Compiler options:
# -mmcu = target MCU (sort of CPU)
# -Wall = all warnings on
# -Werror = Treat warnings as errors
# -Os = optimize for size
# -mcall-prologues = I don't know...
CFLAGS=-g -mmcu=$(MCU) -Wall -Werror -Wstrict-prototypes -Os -mcall-prologues




# Here are avr driver files listed (hopefully don't need to be changed so often).
AVR_OBJECT_FILES=avr_tmr0.o avr_uart.o avr_sys.o
AVR_HEADER_FILES=avr_cfg.h avr_tmr0.h avr_uart.h avr_sys.h
HEADER_FILES=Makefile $(AVR_HEADER_FILES) $(APPLICATION_HEADER_FILES)


#-------------------
all: havrapp.hex havrapp.bin

#-------------------
help: 
	@echo " "
	@echo "Usage: make [all]|clean"
	@echo " "


#-------------------

# Applications

havrapp.hex : havrapp.out 
	$(OBJCOPY) -R .eeprom -O ihex havrapp.out havrapp.hex 
havrapp.out : $(AVR_OBJECT_FILES) $(APPLICATION_OBJECT_FILES)
	$(CC) $(CFLAGS) -o havrapp.out -Wl,-Map,havrapp.map $(AVR_OBJECT_FILES) $(APPLICATION_OBJECT_FILES)



havrapp.bin : havrapp.hex
	hex2bin havrapp.hex

# Components (tell how to compile ".c" files into ".o" files).

%.o : %.c $(HEADER_FILES) 
	$(CC) $(CFLAGS) -c $< -o $@


# If indivitual c modules need special treatment add them here:
#dcdc.o : dcdc.c $(HEADER_FILES)
#	$(CC) $(CFLAGS) -Os -c dcdc.c


#-------------------
clean:
	rm -f *.o *.map *.out *t.hex
#-------------------
