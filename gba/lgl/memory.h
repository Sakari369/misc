/*
 * memory.h: Memory related stuff.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_MEMORY_H
#define LGL_MEMORY_H

#include "typedefs.h"

/* These are used to place variables or functions in spefic parts of the
   GBA memory. Use like this: 
   IN_EWRAM u16 variable; 
*/
#define IN_EWRAM __attribute__ ((section(".ewram")))
#define IN_IWRAM __attribute__ ((section(".iwram")))
#define CODE_IN_IWRAM __attribute__ ((section(".iwram"),long_call))

/* Base memory locations */

/* 256 KB, general use memory */
#define MEM_EW			0x02000000 
/* 32 KB, fast memory */
#define MEM_IW			0x03000000
/* 1024 bytes, input/output stuff */
#define MEM_IO			0x04000000
/* 1024 bytes, palette memory */
/* Stores two 256-color palettes with 16 bits per color (15 is used) */
#define MEM_PAL			0x05000000
/* 96 KB, video memory */
#define MEM_VIDEO		0x06000000

/* Video pages .. just defined to ease code readability */
#define MEM_VID_PAGE1		MEM_VIDEO
#define MEM_VID_PAGE2		(MEM_VIDEO + 0xA0000)

/* 1024 bytes, object attribute memory */
/* Stores 128 8-byte entries */
#define MEM_OAM			0x07000000
/* up to 32 MB, read only memory */
#define MEM_ROM			0x08000000

/* Palette memory locations */
#define MEM_PAL_BG		MEM_PAL
#define MEM_PAL_OBJ		(MEM_PAL + 0x0200)

/* BG memory locations */
#define MEM_BG_CTRL_BASE	(MEM_IO + 0x0008)
#define MEM_BG_HOFF_BASE	(MEM_IO + 0x0010)
#define MEM_BG_VOFF_BASE	(MEM_IO + 0x0012)

/* DMA memory locations */
#define MEM_DMA_SRC_BASE	(MEM_IO + 0x00B0)
#define MEM_DMA_DST_BASE	(MEM_IO + 0x00B4)
#define MEM_DMA_CTRL_BASE	(MEM_IO + 0x00B8)

/* Timer memory locations */
#define MEM_TIMER_DATA_BASE	(MEM_IO + 0x0100)
#define MEM_TIMER_CTRL_BASE	(MEM_IO + 0x0102)

/* Function prototypes */

void memCopy(void *dst, const void *src, int len);
void memSet(void *dst, int c, int len);
void dmaCopy(void *dst, const void *src, int len);

#endif /* LGL_MEMORY_H */
