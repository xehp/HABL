/*
avr_sys.c

provide functions to set up hardware
For a list of available signals see:
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

2005-02-16 Created. Henrik Bjorkman
2005-02-20 Set up timer0 interrupt. Henrik Bjorkman
2005-02-22 Using timer1 to generate 38 MHz. Will need that to send IR later. Henrik Bjorkman
2005-02-22 Using timer2 to get a timer with a 1 second resolution. /Henrik Bjorkman
2005-02-22 Will try to use timer2 interrupt to make a software uart together with timer1
2005-07-01 cleanup. /Henrik
2005-07-02 Moved timer drivers to own files. /Henrik
*/

// When reading 16bit reg remember to read low byte first then high byte
// when writing write high byte first then low byte but hopefully the C compiler does this.


// includes

#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>

#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"





// local variables and defines


/* compatibilty macros for old style */
/* for new design instead of "sbi(DDRC,PC5);" use "DDRC|= _BV(PC5);" */
#if 0

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

#endif




// This function will set CPU in idle mode, saving energy.
inline void avr_idle(void) 
{
#ifdef __AVR_ATmega8__

  // enable sleep
  MCUCR|=(1<<SE);
  
  // if needed, reduce master clock speed here
  // using XDIV register

  // if needed, turn of current for analog comparator here
    
  // set sleep mode to idle
  MCUCR&=~(7<<SM0);
  //MCUCR|=(0<<SM0);
  asm ("sleep");

  // we continue here after beeing wakened up by an interrupt.

  // reset clock speed to normal value
  // using XDIV register

  // turn on current for analog comparator here (if we turned it off)

  // disable sleep
  MCUCR&=~(1<<SE);
#elif defined (__AVR_ATmega88__)

  // enable sleep
  SMCR|=(1<<SE);
  
  // if needed, reduce master clock speed here
  // using XDIV register

  // if needed, turn of current for analog comparator here
    
  // set sleep mode to idle
  SMCR&=~(7<<SM0);
  //MCUCR|=(0<<SM0);
  
  // Here we go to sleep
  asm ("sleep");

  // we continue here after beeing wakened up by an interrupt.

  // reset clock speed to normal value
  // using XDIV register

  // turn on current for analog comparator here (if we turned it off)

  // disable sleep
  SMCR&=~(1<<SE);

#else
#error unknown target
#endif
}






// busy-wait approximatly ms milliseconds (if function is not interrupted)
#if 1
void avr_delay_ms(int ms) 
{
  int i;
  while(ms>0)
  {
    for (i=0; i<(AVR_FOSC/6000); i++)
    {
      asm ("nop"); 
    }
    ms--;
  }
}
#else
void avr_delay_ms(int ms) 
{
  while(ms>0)
  {
    delay_us(990);
    ms--;
  }
}
#endif

void avr_delay_us(int us) 
{
  while(us>0)
  {
#if AVR_FOSC==16000000L
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
#elif AVR_FOSC==8000000L
      asm ("nop"); 
#else
#error selected AVR_FOSC not supported by delay_us
#endif
    us--;
  }
}





// set up hardware (port directions, registers etc.)
void avr_init() 
{

  // Analog comparator
  ACSR|=1<<ACD; // disable analog comparator to save power


  sei(); // enable global interrupts

  
}




