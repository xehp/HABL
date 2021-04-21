/*
avr_cfg.h

provide functions to set up hardware

Copyright (C) 2021 Henrik Bjorkman www.eit.se/hb.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License version 2.1.

Removing this comment or the history section is not allowed.
If you modify this code make a note about it in the history
section below. That is required!

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

History

2005-03-23 Created by Henrik Bjorkman
2007-03-15 Modified for use in boot loader.
2021-04-20 Using atmega328p at 16 Mhz. /Henrik
*/


#ifndef AVR_CFG_H
#define AVR_CFG_H

// A note about code size:
// Using avr-gcc 4.3.2 the code is not as small as it was using 4.1.2
// The version string and the CRC check no longer both fit in 512 words.
// So there are three options here.
// 1) Use old compiler or find/set code size options.
// 2) Remove WELCOME_STRING (comment WELCOME_STRING out below)
// 3) Don't use CRC (comment use macro IGNORE_CSUM below)
// 4) Make boot sector larger (1024 words)
// If you wish to use a 1024 word bootloader section you also need to change
// fuse bits see avrdude_usb_atmega328p.txt and in the makefile.
// Current version will use 1024 word bootloader.

#define WELCOME_STRING "\r\nwww.eit.se/habl/"VERSION_STRING"\r\n"

#define AVR_FOSC 16000000L

// Recommended baud rates are 300, 9600 or 19200,
// If FOSC is 16 MHz and 115200 is selected here we get 125000 baud instead.
#define UART_BAUDRATE 9600L

// Define this if boot loader shall enable watchdog timer
#define ENABLE_WDT

// A major problem with XMODEM is to know if its 16bit CRC or 1 byte simple checksum.
// cutecom and hyperterminal seems to use 16bit CRC. Many others 1 byte simple checksum.
#define USE_XMODEM_CRC16

// The program might not fit in 512 word boot loader unless checksum is ignored.
//#define IGNORE_CSUM



#endif
