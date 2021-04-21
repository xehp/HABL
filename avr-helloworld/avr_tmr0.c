/*
avr_tmr0.c 

provide functions to set up hardware
For a list ov available signals see:
http://www.nongnu.org/avr-libc/user-manual/index.html

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

2005-07-02 created using code from avr_misc.c. /Henrik Bjorkman

*/

// When reading 16bit reg remember to read low byte first then high byte
// when writing write high byte first then low byte but hopefully the C compiler does this.


// includes

#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>

#include "avr_cfg.h"
#include "avr_tmr0.h"

// local variables and defines, use 64 bit here to avoid issues with wrapping counters.
volatile int64_t timer0count;

// SIG_OVERFLOW0 renamed to TIMER0_OVF_vect according to:
// http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html

SIGNAL (TIMER0_OVF_vect)
{
  timer0count++;
}

int64_t avr_tmr0_get_tick_64(void)
{
	int64_t tmp;
	TIMSK0&=(unsigned char)~(1<<TOIE0); // disable interrupt, I hope
	tmp=timer0count;
	TIMSK0|=(1<<TOIE0); // enable interrupt
	return tmp;
}

// If only 16 bits are needed use this one instead of avr_tmr0_get_tick_64, it will be faster.
int16_t avr_tmr0_get_tick_16(void)
{
	int16_t tmp;
	TIMSK0&=(unsigned char)~(1<<TOIE0); // disable interrupt, I hope
	tmp=timer0count;
	TIMSK0|=(1<<TOIE0); // enable interrupt
	return tmp;
}

// set up hardware (port directions, registers etc.)
void avr_tmr0_init(void) 
{
	// set up timer 0
	#if AVR_TMR0_DIVIDER == (1024L*256L)
	TCCR0B=0x05; // internal clock with prescaler to 1024 gives interrupt frequency=AVR_FOSC/(1024*256)
	TIMSK0|=(1<<TOIE0); // enable interrupt
	#else
	#error
	#endif
}



