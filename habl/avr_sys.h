/*
avr_sys.h

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

2005-02-16 Created. /Henrik Bjorkman
2005-07-01 cleanup. /Henrik
*/


#ifndef AVR_MISC_H
#define AVR_MISC_H


#ifndef NULL
#define NULL 0
#endif




// set up hardware (port directions, registers etc.)
void avr_init(void);


// busy-wait ms milliseconds or microseconds (if function is not interrupted)
void avr_delay_ms(int ms);
void avr_delay_us(int us);

// Enter power save mode
inline void avr_idle(void);


#endif // AVR_H
