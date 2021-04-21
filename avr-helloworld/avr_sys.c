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

For a list ov available signals see:
http://www.nongnu.org/avr-libc/user-manual/index.html

History

2005-02-16 Created. Henrik Bjorkman
2005-02-20 Set up timer0 interrupt /Henrik Bjorkman
2005-02-22 Using timer1 to generate 38 MHz. Will need that to send IR later. /Henrik Bjorkman
2005-02-22 Using timer2 to get a timer with a 1 second resolution Henrik Bjorkman
2005-02-22 Will try to use timer2 interrupt to make a software uart together with timer1
2005-07-01 cleanup Henrik
2005-07-02 Moved timer drivers to own files. Henrik

*/

// When reading 16bit reg remember to read low byte first then high byte
// when writing write high byte first then low byte but hopefully the C compiler does this.


// includes

#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_tmr0.h"

#if ((defined(__AVR_ATmega88__)) || (defined(__AVR_ATmega168__)) || (defined(__AVR_ATmega328P__)))
// OK
#else
#warning // It should work with other atmega mcus to but that is not tested.
#endif



#define DEBUG_UART_OUTBUF_EMPTY() (!( UCSR0A & (1<<UDRE0)))
#define DEBUG_UART_INBUF_EMPTY() (!(UCSR0A & (1<<RXC0)))


// local variables and defines






#ifdef DEBUG_LED_PORT

// Flash n times with the LED, just to see that CPU is running before doing anything else.
// This may be called before debug_init has been called.
int avr_blink(char n)
{
	avr_delay_ms_16(200);

	/* INITIALIZE */

	/* set output to off */
	DEBUG_LED_OFF();

	/* enable PC5 as output */
	DEBUG_LED_ENABLE();

	/* BLINK, BLINK ... */

	while (n>0)
	{
		/* set LED on */
		DEBUG_LED_ON();

		avr_delay_ms_16(200);

		/* set LED off */
		DEBUG_LED_OFF();

		avr_delay_ms_16(800);

		n--;
	}

	/* disable debug LED as output */
	DEBUG_LED_DISABLE();

	avr_delay_ms_16(1000);

	return 0;
}

#endif



// This function will set CPU in idle mode, saving energy.
void avr_idle(void) 
{
	wdt_reset();

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
}


#if 0
// busy-wait approximately in milliseconds (if function is not interrupted)
void avr_delay_ms_16(int delay_ms)
{
	int i;
	while(delay_ms>0)
	{
		for (i=0; i<(AVR_FOSC/6000L); i++)
		{
			wdt_reset(); // If wdt_reset isn't wanted, put a no OP here instead: asm ("nop");
		}
		delay_ms--;
	}
}
#elif 0
// alternative function using the avr_delay_us below
void avr_delay_ms_16(int delay_ms)
{
	while(delay_ms>0)
	{
		avr_delay_us(990);
		delay_ms--;
	}
}
#else
// Less accurate busy wait but will save power by using idle and timer0.
void avr_delay_ms_16(int delay_ms)
{
	// Translate into timer0 ticks
	int16_t ticks_to_delay = AVR_TMR0_TRANSLATE_MS_TO_TICKS_16(delay_ms);
	int16_t tick_target=avr_tmr0_get_tick_16()+ticks_to_delay;
	while((tick_target-avr_tmr0_get_tick_16())>=0)
	{
		// wait
		wdt_reset();
		avr_idle();
	}
}

#endif

/**
 * Subtract about 10 us just for the time calling this will take.
 * For better precision if making pulses disable interrupt when using this.
 * cli(); // disable global interrupts
 * sei(); // enable global interrupts
 */
void avr_delay_us(int delay_us)
{
	if (delay_us==0)
	{
		return;
	}
	else
	{
		delay_us--;
		while(delay_us>0)
		{
			#if AVR_FOSC==8000000L
			asm ("nop");
			#elif AVR_FOSC==10000000L
			asm ("nop");
			asm ("nop");
			asm ("nop");
			#elif AVR_FOSC==12000000L
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			#elif AVR_FOSC==16000000L
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			#elif AVR_FOSC==20000000L
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			#else
			#error selected AVR_FOSC not (yet) supported by delay_us
			#endif
			delay_us--;
		}
	}
}

// set up hardware (port directions, registers etc.)
void avr_init() 
{
	// Analog comparator
	//ACSR|=1<<ACD; // disable analog comparator to save power

	sei(); // enable global interrupts
}

int64_t avr_systime_ms_64(void)
{
	return AVR_TMR0_TRANSLATE_TICKS_TO_MS_64(avr_tmr0_get_tick_64());
}

int16_t avr_systime_ms_16(void)
{
	return AVR_TMR0_TRANSLATE_TICKS_TO_MS_16(avr_tmr0_get_tick_64());
}
