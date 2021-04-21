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
2021-04-20 Updated. /Henrik
*/


#ifndef AVR_CFG_H
#define AVR_CFG_H

#define AVR_FOSC 16000000L

// Recommended baud rates are 300, 9600 or 19200,
// If FOSC is 16 MHz and 115200 is selected here we get 125000 baud instead.
#define UART_BAUDRATE 9600


// Using pin PD3 for debug LED
//#define DEBUG_LED_DDR DDRD
//#define DEBUG_LED_PORT PORTD
//#define DEBUG_LED_BIT PD3


#endif
