/*

posprintf - a condensed version of sprintf for Thumb, esp. GBA
Copyright (C) 2003 Dan Posluns

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 

Author contact e-mail: dan at danposluns dot com


INSTRUCTIONS:

To call: posprintf(char *dest, const char *src[, param1[, param2[, ... paramN]]]);

- src must be a valid zero-terminated C string.
- dest must point to a sufficiently large block of memory to contain the result string.

The following format specifiers are valid:

%% - print a '%' symbol
%s - print a string; parameter must point to a valid zero-terminated C string
%d - print a 16-bit (short) integer; must be within [-65,535 .. 65,535]
%l - print a 29-bit integer; approximate range [-500,000,000 .. 500,000,000]
%x - print a hexadecimal number (lowercase digits)
%X - print a hexadecimal number (uppercase digits)

The specifiers %d, %l, %x and %X may be modified as follows:

- Digits 1 through 9 indicate number of leading spaces to print, eg.
	%5d would print the number 123 as "  123"
	%5d would print the number 123456 as "123456" (no leading spaces)
- When above digit is prefixed with 0, leading zeros are printed instead of spaces
	%05d would print the number 123 as "00123"
	%04d would print the number 12345 as "12345" (no leading zeros)
- Negative sign consumes a leading space, eg.
	%05d would print the number -123 as "-0123"
	(Hexadecimal numbers are considered unsigned)

IF YOU WANT MORE FUNCTIONALITY THAN THIS, YOU CAN FEEL FREE TO MODIFY THE CODE,
BUT THEN I WOULD SUGGEST MOVING TO A MORE SOPHISTICATED LIBRARY ANYWAY.

*** CAUTION IF NOT USED ON GAMEBOY ADVANCE ***
Although this is mostly written as general Thumb code, the %l (29-bit print)
specifier code currently uses a software interrupt (SWI) specific to the
Gameboy Advance to perform a division. If you wish to port this to other ARM
machines, you may need to alter this code. I believe that most ARM machines
support SWI 7 as an alternative software divide, although you will have to
swap the numerator/denominator registers (r0 and r1).
*** END CAUTION ***

Although this is distributed as free software under the GNU LGPL, please feel
free to let me know if you use posprintf in your project, and to credit me if
you use it to make the next blockbuster. :)

My contact e-mail is: dan at danposluns dot com

*/

#ifndef _PSPRINTF_HEADER_
#define _PSPRINTF_HEADER_

extern void posprintf(char *, const char *, ...);

#endif

