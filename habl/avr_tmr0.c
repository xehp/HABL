/*
avr_misc.c 

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

For a list of available signals see:
http://www.nongnu.org/avr-libc/user-manual/index.html

History

2005-07-02 created using code from avr_misc.c /Henrik Bjorkman

*/

// When reading 16bit reg remember to read low byte first then high byte
// when writing write high byte first then low byte but hopefully the C compiler does this.


// includes

#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>

#include "avr_cfg.h"
#include "avr_tmr0.h"
#include "avr_sys.h"

// local variables and defines




volatile int timer0count;


SIGNAL (SIG_OVERFLOW0)
{
  timer0count++;

  if (timer0count&0x100)
  {
    PORTC &= ~_BV(PC5);
  }
  else
  {
    PORTC|= _BV(PC5);
  }
}

int avr_tmr0_gettick(void)
{
  int tmp;
#ifdef __AVR_ATmega8__
  TIMSK&=(unsigned char)~(1<<TOIE0); // disable interrupt, I hope
  tmp=timer0count;
  TIMSK|=(1<<TOIE0); // enable interrupt
#elif defined __AVR_ATmega88__
  TIMSK0&=(unsigned char)~(1<<TOIE0); // disable interrupt, I hope
  tmp=timer0count;
  TIMSK0|=(1<<TOIE0); // enable interrupt
#else
#error
#endif
  return tmp;
}

// set up hardware (port directions, registers etc.)
void avr_tmr0_init(void) 
{
  // set up timer 0
#ifdef __AVR_ATmega8__
  TCCR0=0x05; // internal clock with prescaler to 1024 gives interrupt frequency=AVR_FOSC/(1024*256)
  TIMSK|=(1<<TOIE0); // enable interrupt 
#elif defined __AVR_ATmega88__
  TCCR0B=0x05; // internal clock with prescaler to 1024 gives interrupt frequency=AVR_FOSC/(1024*256)
  TIMSK0|=(1<<TOIE0); // enable interrupt 
#else
#error
#endif
}








