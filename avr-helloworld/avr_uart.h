/*
avr_uart.h

Device driver for hardware UART on AVR platform.

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

2005-02-16 Modified for atmega8 and renamed. Used some code from an GPL avr platform written by Lukas Karrer. /Henrik Björkman
2005-07-01 cleanup. /Henrik
2005-07-09 Renamed uart_packet to uart_buf. /Henrik
2005-07-09 Renamed uart_packet to uart_buf, moved it to its own file. Henrik
2012-05-12 Removed buffered mode. Keeping only stream mode. /Henrik
2012-05-21 Restored some extra features. Henrik
2016-08-02 renamed the functions for compatibility with another project. /Henrik

*/

#ifndef AVR_UART_H
#define AVR_UART_H

#include "avr_cfg.h"



#ifndef UART_BAUDRATE
#error
#endif

// size of buffer for incoming Data
// NOTE it must be power of 2: 1,2,4,8,16,32,64,128 or 256 bytes
#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE 64
#endif

// size of buffer for outgoing data
// NOTE it must be power of 2: 1,2,4,8,16,32,64,128 or 256 bytes
#ifndef UART_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE 32
#endif



/* 
set up Hardware UART
PRE: Interrupts are enabled
POST: Hardware UART is intitialized for receiving and transmitting 
      with interrupt
*/
void uart_init(void);




// This will tell if there is data available in the receive buffer.
// Returns Number of of available bytes in RX buffer
unsigned char uart_get_nbytes_in_read_buffer(void);


// Returns Number of unsent bytes in TX buffer
unsigned char uart_get_n_unsent_bytes_in_write_buffer(void);


// Returns Number of bytes that can be written without blocking
unsigned char uart_get_free_space_in_write_buffer(void);

// This gives a counter that is incremented if/when input buffer overflows.
unsigned char uart_get_overflow_counter(void);


// write n bytes of *data into transmit buffer and start transmitting
// If there is not room for all in out buffer this call will block until there is room.
void uart_write(char *data_ptr, int n_data);

// Prints a string from program memory.
void uart_print_P(const char *addr);

// Check and get a character, returns -1 if no character was available.
int uart_getchar(void);

// Sends a character on uart, waits until there is room for it in buffer if buffer is full.
void uart_putchar(char ch);

// Wait fo a character to be available and get it.
unsigned char uart_waitchar(void);



#endif // AVR_UART_H








