/*
xmodem.h

Copyright (C) 2021 Henrik Bjorkman www.eit.se/hb.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License version 2.1.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

If you modify this code make a note about it in the history
section below.

History

2007-03-15
Created
Henrik Bjorkman

*/


#ifndef XMODEM_H
#define XMODEM_H


#ifndef NULL
#define NULL 0
#endif


int xmodem_calcrc(char *ptr, int count);
int xmodem(void);


#endif // AVR_H
