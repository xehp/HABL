# makefile, written by guido socher
# downloaded from http://linuxfocus.org/English/November2004/article352.shtml
# modified 2005-02-16 by Henrik Bjorkman to fit my application

PROJ_NAME=habl

HEADER_FILES=avr_cfg.h avr_uart.h xmodem.h version.h
OBJECT_FILES=habl.o avr_uart.o xmodem.o



# Change here if you have another atmega microcontroller (MCU)
# Remember to adjust START_ADRESS_OF_BOOT_SECTION also!
#MCU=atmega88
#MCU=atmega168
MCU=atmega328p


# Size of boot loader section. This is in bytes, Atmega documentation will use words.
#   You may need to adjust this for:
#   atmega88 with 512 word boot section it shall be 0x01C00.
#   atmega88 with 1024 word boot section its probably  0x01800 (I have not tested this)
#   atmega168 with 512 word boot section it shall be 0x03C00 (512 words is 1024 bytes 0x4000-0x400=0x3C00)
#   atmega168 with 1024 word boot section its probably  0x03800 (I have not tested this)
#   atmega328 with 512 word boot section its probably  0x07800 (0x8000-0x400=0x7C00)
#   atmega328 with 1024 word boot section its probably  0x07800 (0x8000-0x800=0x7800)
# This must match the fuse bits you set, see avrdude_usb_atmega328.txt.
# Also remember to check the IGNORE_CSUM macro in avr_cfg.h.
# if a 512 word boot section is used that macro needs to be defined to reduce code size.
START_ADRESS_OF_BOOT_SECTION=0x7800


# Some generic macros
CC=avr-gcc
OBJCOPY=avr-objcopy

# Give compiler options here
# -Wall = all warnings on
# -Werror = Treat warnings as errors
# -Os = optimize for size
# --section-start = Where to put the program in memory.
CFLAGS=-g -mmcu=$(MCU) -Wall -Werror -Wstrict-prototypes -Os -mcall-prologues\
       -DPROGRAM_MEMORY_SIZE=$(START_ADRESS_OF_BOOT_SECTION)\
       -Wl,--section-start=.text=$(START_ADRESS_OF_BOOT_SECTION)



all: $(PROJ_NAME).hex


help: 
	@echo "Usage: make [all|clean]"
	@echo ""
	@echo ""
	@echo ""


$(PROJ_NAME).hex : $(PROJ_NAME).out 
	$(OBJCOPY) -R .eeprom -O ihex $(PROJ_NAME).out $(PROJ_NAME).hex 
	
$(PROJ_NAME).out : $(OBJECT_FILES)
	$(CC) $(CFLAGS) -o $(PROJ_NAME).out -Wl,-Map,$(PROJ_NAME).map $(OBJECT_FILES)

%.o : %.c $(HEADER_FILES) 
	$(CC) $(CFLAGS) -c $< -o $@



clean:
	rm -f *.o *.map *.out *.hex


