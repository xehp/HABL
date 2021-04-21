/*
avr_uart.h

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

2005-02-16 Modified for atmega8 and renamed. Used some code from an GPL avr platform written by Lukas Karrer. /Henrik Bjorkman
2005-07-01 cleanup Henrik
2005-07-09 only some comments changed. /Henrik
2005-07-09 Renamed uart_packet to uart_buf, moved it to its own file. /Henrik
2012-05-12 Removed buffered mode. Keeping only stream mode. Henrik
2012-05-21 Restored some extra features. Henrik

*/



// We might also want to do:
// Redirecting STDIN and STDOUT to UART
// Read more at http://www.appelsiini.net/2011/simple-usart-with-avr-libc


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
#include <avr/wdt.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>

#include "avr_cfg.h"
#include "avr_uart.h"
#include "avr_sys.h"


#define USE_INTERRUPT_FOR_RECEIVE
#define USE_INTERRUPT_FOR_TRANSMIT








#ifndef UART_BAUDRATE
#error need to know baudrate, typically set it like this: #define UART_BAUDRATE 19200
#endif


#ifndef AVR_FOSC
#error need to know clock frequency to set baudrate, typically: #define AVR_FOSC 16000000L 
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



#define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)
#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)



#ifdef USE_INTERRUPT_FOR_RECEIVE

// buffer to store incoming data
static unsigned char uartRxBuf[UART_RX_BUFFER_SIZE];

// put at the tail.
// get from the head.
// head=tail means empty buffer
// actual number of characters that can be stored is BUFFER_SIZE-1 

// index into ringbuffer, points to the next entry in buffer where data can be stored.
static volatile unsigned char uartRxTail=0;

// index into ringbuffer, points to entry first in queue to be taken care of.
static volatile unsigned char uartRxHead=0;

static volatile unsigned char uartRxOverflowCounter=0;

#endif


#ifdef USE_INTERRUPT_FOR_TRANSMIT

// buffer to store outgoing data
static unsigned char uartTxBuf[UART_TX_BUFFER_SIZE];

// index into ringbuffer, points to the next entry in buffer where data can be stored.
static volatile unsigned char uartTxTail=0;

// index into ringbuffer, points to entry first in queue to be sent.
static volatile unsigned char uartTxHead=0;

static volatile char uartTxBusy=0;

#endif



// A counter incremented everytime there is some error in uart.
//static volatile char UART_ErrorCounter=0;

// Get one byte from input buffer
// Returns -1 if buffer was empty.
int uart_getchar()
{  
#ifdef USE_INTERRUPT_FOR_RECEIVE
// Using interrupt 

  if (uartRxHead == uartRxTail)
  {
    // we do not have any data in buffer
    return(-1);
  }

  //cli(); // disable global interrupts, probably not needed.

  {
    const volatile unsigned char ch = uartRxBuf[uartRxHead];


    /* calculate new buffer index */
    {
      const volatile unsigned char tmp = ( uartRxHead + 1 ) & UART_RX_BUFFER_MASK;
      uartRxHead = tmp; /* store new index */
    }

    //sbi (UCSR0B ,UDRIE0); // enable global interrupts, probably not needed.

    return ch;
  }

#else // #ifdef USE_INTERRUPT_FOR_RECEIVE
  // Not using interrupt

  /* Wait for data to be received */
  if (UART_INBUF_EMPTY())
  {
    wdt_reset();
    return -1;
  }

  /* Get and return received data from buffer */

return UDR0;

#endif // #ifdef USE_INTERRUPT_FOR_RECEIVE
}



// wait until a character is available
unsigned char uart_waitchar()
{  
#ifdef USE_INTERRUPT_FOR_RECEIVE
  // using interrupt.

  while (uartRxHead == uartRxTail)
  {
    // we do not have any data in buffer, wait
    wdt_reset();
    avr_idle(); // set CPU idle to save power
  }

  //cli(); // disable global interrupts, probably not needed.

  {
    const volatile unsigned char ch = uartRxBuf[uartRxHead];

    /* calculate new buffer index */
    {
      const volatile unsigned char tmp = ( uartRxHead + 1 ) & UART_RX_BUFFER_MASK;  
      uartRxHead = tmp; /* store new index */
    }

    //sbi (UCSR0B ,UDRIE0); // enable global interrupts, probably not needed.

    return ch;
  }

#else // #ifdef USE_INTERRUPT_FOR_RECEIVE
  // not using interrupt.

  /* Wait for data to be received */
  while ( UART_INBUF_EMPTY() )
  {
    wdt_reset();
  }

  /* Get and return received data from buffer */
  return UDR0;

#endif // #ifdef USE_INTERRUPT_FOR_RECEIVE
}




// Put one character in output buffer
// wait if buffer is full
void uart_putchar(char ch)
{
#ifdef USE_INTERRUPT_FOR_TRANSMIT
  // using interrupt.

  const volatile unsigned char tmp = ( uartTxTail + 1 ) & UART_TX_BUFFER_MASK;
  
  while(uartTxHead==tmp)
  {
    // buffer if full, must wait
    avr_idle(); // set CPU idle to save power during this busy wait.
  }


  cli(); // disable global interrupts

  if (!uartTxBusy)
  {
    // transmiter is idle, send the char to uart without buffering it.

    UDR0=ch; // start transmition of next byte

    // enable UDRE interrupt
    UCSR0B |= _BV(UCSR0B);
    uartTxBusy=1;
  }
  else
  {
    // put the char in the transmit queue
    uartTxBuf[uartTxTail] = ch; /* store data in buffer, must be done before moving index */
    uartTxTail = tmp; /* store new index */
  }

  sei(); // enable global interrupts

#else // #ifdef USE_INTERRUPT_FOR_TRANSMIT
  // not using interrupt.

  /* Wait for empty transmit buffer */
  while ( UART_OUTBUF_EMPTY() )
  {
    // busy wait
  }

  /* Put data into buffer, sends the data */

  UDR0 = ch;

#endif // #ifdef USE_INTERRUPT_FOR_TRANSMIT
}







unsigned char uart_get_nbytes_in_read_buffer () 
{
#ifdef USE_INTERRUPT_FOR_RECEIVE
    return ( uartRxTail - uartRxHead ) & UART_RX_BUFFER_MASK;
#else
    if (!UART_INBUF_EMPTY())
    {
      return 1;
    }
    return 0;
#endif
}



unsigned char uart_get_n_unsent_bytes_in_write_buffer () 
{
#ifdef USE_INTERRUPT_FOR_TRANSMIT
    return ( uartTxTail - uartTxHead ) & UART_TX_BUFFER_MASK;
#else
    if (!UART_OUTBUF_EMPTY())
    {
       return 1;
    }
    return 0;
#endif
}



unsigned char uart_get_free_space_in_write_buffer () 
{
#ifdef USE_INTERRUPT_FOR_TRANSMIT
  return((UART_TX_BUFFER_SIZE-1)-uart_get_n_unsent_bytes_in_write_buffer());
#else
    if (UART_OUTBUF_EMPTY())
    {
       return 1;
    }
    return 0;
#endif
}


unsigned char uart_get_overflow_counter(void)
{
#ifdef USE_INTERRUPT_FOR_TRANSMIT
  return uartRxOverflowCounter;
#else
  return 0;
#endif
}


// This must be called before any of the other uart functions are called.
void uart_init(void)
{


  // activate internal pull up on PD0, used for serial data input (active low signal).
  PORTD |= _BV(PD0);
  
  // do we need internal pull up on PD1 also?
  //PORTD |= _BV(PD1);



  //ubrr_value = 103;
  unsigned int ubrr_value = (AVR_FOSC/(16L*UART_BAUDRATE))-1; // AVR_FOSC is larger than a 16 bit int so be careful here;
  // Recommended baud rates are 300, 9600 or 19200,
  // With 16 MHz and 115200 baud we get 125000 instead, so be aware.




  /* Set baud rate */
  UBRR0H = (unsigned char)(ubrr_value>>8);
  UBRR0L = (unsigned char)ubrr_value;

  // Setting this bit UCSR0A bit 1 (U2Xn) the baudrate is doubled
  // Remember to adjust calculation of ubrr if this is used.
  //UCSR0A |= _BV(1); // Or is it better like this:  UCSR0A |= _BV(U2X0);
  // Clearing it is default so ignoring this step is possible (but if your boot loader set it things will get confused)
  UCSR0A &= ~_BV(1);

  /* Enable receiver and transmitter */
  UCSR0B = (1<<RXEN0)|(1<<TXEN0); // UCSR0B = _BV(RXEN0) | _BV(TXEN0);

  /* Set frame format: 8data, 2stop bit */
  UCSR0C = (1<<USBS0)|(3<<UCSZ00);

  // URSEL not needed in atmega88?

#ifdef USE_INTERRUPT_FOR_RECEIVE
  // enable RX interrupt
  UCSR0B |= (1<<RXCIE0);
#endif

#ifdef USE_INTERRUPT_FOR_TRANSMIT
  UCSR0B|= _BV(TXCIE0);
#endif

}



#ifdef USE_INTERRUPT_FOR_TRANSMIT
/*
Interrupt routine after byte has been transfered to shift register
and UART data Register UDR is free again
PRE: 
POST: if uartTxBuf is not empty, set up UART to send next byte 
*/


//SIGNAL (SIG_USART_DATA)
ISR(USART_TX_vect)
{
  /* check if all data is transmitted */
  if ( uartTxTail != uartTxHead )
  { 
    /* start transmition of next byte */
    UDR0=uartTxBuf[uartTxHead]; /* start transmition of next byte */

    /* calculate buffer index */
    /* store new index */
    {
      const volatile unsigned char tmp = ( uartTxHead + 1 ) & UART_TX_BUFFER_MASK;
      uartTxHead = tmp;
    }

  }
  else
  {
    uartTxBusy=0;
    // disable UDRE interrupt
    UCSR0B &= ~_BV(UCSR0B);
  }
}


#endif





#ifdef USE_INTERRUPT_FOR_RECEIVE
/*
SIG_UART_RECV
Interrupt routine for incoming byte on HW UART
received data is placed in uartRxBuf
*/

//SIGNAL (SIG_USART_RECV)
ISR(USART_RX_vect)
{
  /* read the received data */
  /* store received data in buffer */
  uartRxBuf[uartRxTail] = UDR0;

  /* calculate buffer index */
  {
    const unsigned char tmphead = ( uartRxTail + 1 ) & UART_RX_BUFFER_MASK;
    uartRxTail = tmphead; /* store new index */
 
    if ( tmphead == uartRxHead )
    {
      // ERROR! Receive buffer overflow, not handled.
      uartRxOverflowCounter++; // just counted
    }
  }


}


#endif




void uart_write(char *data_ptr, int n_data) 
{
  while( n_data > 0) 
  { 
    uart_putchar(*data_ptr);
    data_ptr++;
    n_data--;
  }
}




/* debug_P */
/* In order to print a string stored in program memory use this function */
/* The call should look like this: uart_debug_P(PSTR("foo was here\n")); */
void uart_print_P(const char *addr)
{
  char c; 
  while ((c = pgm_read_byte(addr++))) 
  {
    uart_putchar(c);
  }
}






