/*
avr_uart.h

Provide functions to set up uart.

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

2005-02-16 Modified for atmega8 and renamed. Henrik Bjorkman
2007-03-15 Modified for atmega88. Henrik Bjorkman
2012-05-14 Corrected baudrate on ATMEGA88. Henrik Bjorkman
2012-05-22 Small fix to compile on atmega168. Henrik Bjorkman

*/


/* -------------------------------------------------------------------------
 * BUGS
 * ------------------------------------------------------------------------- */
/*
// port_or is not defined in iomacro.h (which gets included in io.h) while
// __port_or has a definition. This seems to be a bug in iomacro.h
#define port_or __port_or
// ditto
#define port_and __port_and
*/

/* -------------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------------- */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "avr_cfg.h"
#include "avr_uart.h"

/* compatibilty macros for old style */
/* for new design instead of "sbi(DDRC,PC5);" use "DDRC|= _BV(PC5);" */
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef outp
#define outp(value,sfr) (_SFR_BYTE(sfr) = value)
#endif

#ifndef inp
#define inp(sfr) (_SFR_BYTE(sfr))
#endif



/* -------------------------------------------------------------------------
 * local variables and defines
 * ------------------------------------------------------------------------- */




#if ((defined __AVR_ATmega88__) || (defined __AVR_ATmega168__) || (defined __AVR_ATmega328P__))
// OK
#else
#error Unknown or not yet supported device
#endif


#define UART_OUTBUF_EMPTY() (!( UCSR0A & (1<<UDRE0)))
#define UART_INBUF_EMPTY() (!(UCSR0A & (1<<RXC0)))




void uart_init(void)
{

	// AVR_FOSC is larger than a 16 bit int so be careful here;
	// Recommended baud rates are 300, 9600 or 19200,
	// With 16 MHz and 115200 baud we get 125000 instead, so be aware.
#if UART_BAUDRATE >=9600
	// Setting this bit UCSR0A bit 1 (U2Xn) the baudrate is doubled,
	// This can be used to get 115200 baud with 20 MHz and only 1% off.
	// Baudrate will be: AVR_FOSC/(8*(ubrr_value+1))
	UCSR0A |= _BV(U2X0);
	uint16_t ubrr_value = ((AVR_FOSC+(4L*UART_BAUDRATE))/(8L*UART_BAUDRATE))-1;
	//uint16_t ubrr_value = 7;
#else
	// Clearing U2Xn is default so ignoring this step is possible
	// (but if your boot loader set it things will get confused)
	// This is the boot loader so skip it.
	//UCSR0A &= ~_BV(U2X0);
	uint16_t ubrr_value = (AVR_FOSC/(16L*UART_BAUDRATE))-1;
#endif



  /* Set baud rate */
  UBRR0H = (unsigned char)(ubrr_value>>8);
  UBRR0L = (unsigned char)ubrr_value;


  /* Enable receiver and transmitter */
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);

  /* Set frame format: 8data, 2stop bit */
  UCSR0C = (1<<USBS0)|(3<<UCSZ00);

  // URSEL not needed in atmega88?

  /* enable RX interrup */
  //UCSR0B |= (1<<RXCIE0);

  // activate internal pull up on PD0, used for serial data input (active low signal).
  PORTD |= _BV(PD0);

}



/* send if not using interrupt */
void uart_putchar( unsigned char data )
{
  /* Wait for empty transmit buffer */
  while ( UART_OUTBUF_EMPTY() )
  {
#ifdef ENABLE_WDT
    wdt_reset();
#endif
  }

  /* Put data into buffer, sends the data */
  UDR0 = data;
}


#if 0
/* receive if not using interrupt */
/* wait until a character is available */
unsigned char uart_waitchar( void )
{
  /* Wait for data to be received */
  while ( UART_INBUF_EMPTY() )
  {
#ifdef ENABLE_WDT
    wdt_reset();
#endif
  }

  /* Get and return received data from buffer */
#ifdef __AVR_ATmega8__
  return UDR;
#elif (defined __AVR_ATmega88__ || __AVR_ATmega168__)
  return UDR0;
#else
#error
#endif
}
#endif

/* receive if not using interrupt */
/* return -1 if no character was available */
int uart_getchar(void)
{
  /* Wait for data to be received */
  if (UART_INBUF_EMPTY())
  {
#ifdef ENABLE_WDT
    wdt_reset();
#endif
	return -1;
  }

  /* Get and return received data from buffer */
  return UDR0;
}


#ifdef WELCOME_STRING
void uart_print_P(const char *addr)
{
  char c; 
  while ((c = pgm_read_byte(addr++))) 
  {
    uart_putchar(c);
  }
}
#endif

