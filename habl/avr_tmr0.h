/*
avr_tmr0.h

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

2005-07-02 created using code from avr_misc.h. /Henrik Bjorkman
*/


#ifndef AVR_TMR0_H
#define AVR_TMR0_H



#define AVR_TMR0_TICKS_PER_SEC (AVR_FOSC/(1024*256))

// set up hardware (port directions, registers etc.)
void avr_tmr0_init(void);

int avr_tmr0_gettick(void);




#endif
