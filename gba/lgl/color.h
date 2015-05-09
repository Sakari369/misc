/*
 * color.h: GBA color helper stuff.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_COLOR_H
#define LGL_COLOR_H

#include "typedefs.h"


// Masks for components of a 15-bit color value
#define BITMASK_R	0x1F
#define BITMASK_G	0x3E0
#define BITMASK_B	0x7C00
// Bitmask for 15-bit color
#define BITMASK_RGB	0x7FFF
#define BITMASK_RGB_H	0x7FFF0000
#define BITMASK_RGB_L	0x00007FFF

// Getting 
#define COLOR32_GET_15B_H(ptr) (((ptr) & BITMASK_RGB_H) >> 4)
#define COLOR32_GET_15B_L(ptr) (((ptr) & BITMASK_RGB_L))

/* Forms a RGB 15-bit 5,5,5 BGR format color suitable for GBA */
#define RGB(r, g, b)	((u16)((r) | ((g) << 5) | ((b) << 10)))
// Mask out each color component from a 15-bit color
#define MASK_BLUE(c)	((u16)((c & BITMASK_B) >> 10))
#define MASK_GREEN(c)	((u16)((c & BITMASK_G) >> 5))
#define MASK_RED(c)	((u16)((c & BITMASK_R)))

/* Some predefined colours. Apparently you shouldn't use component values */
/* below 8, because they show too dark on the LCD screen. Shouldn't have this */
/* problem with the GBA SP ? */

#define COLOR_WHITE	RGB(31, 31, 31)
#define COLOR_BLACK	RGB(0, 0, 0)
#define COLOR_RED	RGB(31, 8, 8)
#define COLOR_ORANGE	RGB(31, 23, 8)
#define COLOR_YELLOW	RGB(31, 31, 8)
#define COLOR_GREEN	RGB(8, 31, 8)
#define COLOR_TURQUOISE	RGB(8, 31, 31)
#define COLOR_BLUE	RGB(8, 8, 31)
#define COLOR_MAGENTA	RGB(31, 8, 31)
#define COLOR_BROWN	RGB(23, 17, 8)

#endif /* LGL_COLOR_H */
