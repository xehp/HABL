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

2005-07-02 created using code from avr_misc.h Henrik Bjorkman
*/


#ifndef AVR_TMR0_H
#define AVR_TMR0_H

#define AVR_TMR0_DIVIDER (1024L*256L)
#define AVR_TMR0_TICKS_PER_SEC (AVR_FOSC/AVR_TMR0_DIVIDER)

#define AVR_TMR0_TRANSLATE_MS_TO_TICKS_64(time_ms) ((((int64_t)time_ms * (AVR_FOSC/1000L))) / AVR_TMR0_DIVIDER)
#define AVR_TMR0_TRANSLATE_TICKS_TO_MS_64(ticks) ((((int64_t)ticks * AVR_TMR0_DIVIDER)) / (AVR_FOSC/1000L))
#define AVR_TMR0_TRANSLATE_MS_TO_TICKS_16(time_ms) ((((AVR_FOSC/1000L) * time_ms)) / AVR_TMR0_DIVIDER)
#define AVR_TMR0_TRANSLATE_TICKS_TO_MS_16(ticks) (((AVR_TMR0_DIVIDER * ticks)) / (AVR_FOSC/1000L))



// set up hardware (port directions, registers etc.)
void avr_tmr0_init(void);

int64_t avr_tmr0_get_tick_64(void);
int16_t avr_tmr0_get_tick_16(void);

#endif
