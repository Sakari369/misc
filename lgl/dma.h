/*
 * dma.h: DMA definitions and related stuff.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_DMA_H
#define LGL_DMA_H

#include "typedefs.h"
#include "registers.h"

#define DMA_ENABLE		0x80000000
#define DMA_IRQ			0x40000000
/* Destination adjustment modes */
#define DMA_DST_INC		0
#define DMA_DST_DEC		0x00200000
#define DMA_DST_FIXED		0x00400000
#define DMA_DST_RESET		0x00600000
/* Source adjustment modes */
#define DMA_SRC_INC		0
#define DMA_SRC_DEC		0x00800000
#define DMA_SRC_FIXED		0x01000000
#define DMA_SRC_RESET		0x00600000
/* When the transfer should start*/
#define DMA_START_NOW		0
#define DMA_START_VBLANK	0x10000000
#define DMA_START_HBLANK	0x20000000
#define DMA_START_REFRESH	0x30000000
/* Chunksizes*/	 
#define DMA_CS16		0
#define DMA_CS32		0x04000000

/* Bit definitions for DMA control registers */

/* Macro to set up an DMA transfer, copies from src to dst using channel */
/* chan and controlling options ctrl. TODO: doesn't work .. */
#if 0

#define DMA_TRANSFER(dst, src, count, chan, ctrl) \
{						\
	R32_SRC_DMA##chan = (u32)src; \
	R32_DST_DMA##chan = (u32)dst; \
	R32_CTRL_DMA##chan = (count | ctrl); \
}
#endif

/* Inline functions */

/* Uses DMA to copy data from src to dst. Count is the number of transferred
 * 32-bit chunks. */
INLINE void _dmaCopy(void *dst, const void *src, u16 count)
{
	/* Shut off previous transfer */
	R32_CTRL_DMA3 = 0;
	/* Set source and dest and start copy */
	R32_SRC_DMA3 = (u32)src;
	R32_DST_DMA3 = (u32)dst;
	R32_CTRL_DMA3 = (count | (DMA_ENABLE | DMA_CS32 | DMA_START_NOW));
}

#endif /* LGL_DMA_H */
