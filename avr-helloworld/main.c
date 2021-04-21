/*
main.c

Henriks AVR application

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

2005-02-14 want to use serial port. Have adapted some code I found at http://www.lka.ch/projects/avr/ Henrik Bjorkman
2005-02-20 Will try to interpret some commands Henrik

*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_tmr0.h"
#include "version.h"






int16_t mainTimer=0;
int16_t mainCounter=0;






int main( void ) 
{
	wdt_enable(WDTO_2S);

	uart_init();
	uart_print_P(PSTR("\r\n" VERSION_STRING "\r\n"));
	wdt_reset();

	avr_init();
	avr_tmr0_init();

	#ifdef DEBUG_LED_PORT
	avr_blink(2); // just so we see that it started
	#else
	avr_delay_ms_16(200);
	#endif

	wdt_reset();



	//AVR_ADC_init();

	#ifdef AVR_TIMER1_FREQUENCY
	avr_tmr1_init();
	#endif

	/*
	#ifdef AVR_TIMER2_TICKS_PER_SEC
	avr_tmr2_init();
	#else
	#error
	#endif
	*/

	/*
	#ifdef DEBUG_TEST
	debug_init();
	#endif
	*/




	mainTimer = avr_systime_ms_16() + 1000;


	for(;;)
	{
		const int16_t t = avr_systime_ms_16();

		int ch = uart_getchar();
		if (ch >= 0)
		{
			// Just echo input.
			uart_putchar(ch);
			mainTimer = t+10000;
		}

		if ((mainTimer-t)<0)
		{
			// No input, send something.
			uart_putchar('.');
			mainTimer += 1000;
		}



		wdt_reset();

		// Set CPU in idle mode to save energy, it will wake up next time there is an interrupt
		avr_idle();
	}



	return(0);
} // end main()



