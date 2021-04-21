/*
avr_uart.h

provide functions to set up uart

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

History:

2005-02-16 Modified for atmega8 and renamed. Henrik Bjorkman
2007-03-15 Modified for atmega88. Henrik Bjorkman
2021-04-20 Using atmega328p

*/

#ifndef AVR_UART_H
#define AVR_UART_H


void uart_init(void);
void uart_putchar( unsigned char data );
//unsigned char uart_waitchar(void);
int uart_getchar(void);

void uart_print_P(const char *addr);






#endif // AVR_UART_H
