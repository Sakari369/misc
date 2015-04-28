/*
 * interrupt.h: GBA interrupt definitions.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#include "typedefs.h"
#include "registers.h"

#ifndef LGL_INTERRUPT_H
#define LGL_INTERRUPT_H

#define INT_VBLANK	0x0001
#define INT_HBLANK	0x0002
#define INT_VCOUNT	0x0004
#define INT_TMR0	0x0008
#define INT_TMR1	0x0010
#define INT_TMR2	0x0020
#define INT_TMR3	0x0040
#define INT_COM		0x0080
#define INT_DMA0	0x0100
#define INT_DMA1	0x0200
#define INT_DMA2	0x0400
#define INT_DMA3	0x0800
#define INT_KEYS	0x1000
#define INT_CART	0x2000

#endif /* LGL_INTERRUPT_H */
