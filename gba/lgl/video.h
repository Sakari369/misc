/*
 * video.h: Video stuff. 
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_VIDEO_H
#define LGL_VIDEO_H

#include "typedefs.h"
#include "memory.h"
#include "registers.h"

/* Screen dimensions */
#define SCREEN_W	240
#define SCREEN_H	160

/* Video modes (defined because magic numbers are bad) */
#define VID_MODE0	0
#define VID_MODE1	1
#define VID_MODE2	2
#define VID_MODE3	3
#define VID_MODE4	4
#define VID_MODE5	5
#define VID_PAGE      	0x0010

/* R16_CTRL_DISP bit definitions */
#define VID_LINEAR		0x0040
#define VID_ENABLE_BG0		0x0100
#define VID_ENABLE_BG1		0x0200
#define VID_ENABLE_BG2		0x0400
#define VID_ENABLE_BG3		0x0800
#define VID_ENABLE_OBJ		0x1000
#define VID_ENABLE_WIN0		0x2000
#define VID_ENABLE_WIN1		0x4000
#define VID_ENABLE_OBJWIN	0x8000

/* R16_STAT_DISP bit definitions */
/* The next three are read-only */
#define VID_STAT_VBLANK		0x0001
#define VID_STAT_HBLANK		0x0002
#define VID_STAT_VCOUNT		0x0004

#define	VID_INT_VBLANK		0x0008
#define VID_INT_HBLANK		0x0010
#define VID_INT_VCOUNT		0x0020

/* Pointers */
//#define VRAM_PTR		((u16 *)MEM_VIDEO)
#define PAL_BG_PTR		((u16 *)MEM_PAL_BG)
#define PAL_OBJ_PTR		((u16 *)MEM_PAL_OBJ)

//u16 *vram_ptr = ((u16 *)MEM_VIDEO);
#define vram_ptr		((u16 *)MEM_VIDEO)

#define VID_DEFAULT_MODE0	(VID_MODE0|VID_ENABLE_OBJ|VID_LINEAR)

/* Inlined functions */
INLINE void setVideoMode(int mode)
{
	R16_CTRL_DISP = mode;
}

/*
INLINE void pageFlip(void)
{
	vram_ptr = (u16 *)((u32)vram_ptr ^ 0xA000);
	R16_CTRL_DISP ^= VID_PAGE;
}
*/

INLINE void waitVsync(void)
{
	while (R16_VCOUNT != 160) ;
	while (R16_VCOUNT != 161) ;
}

INLINE void putPixel(int x, int y, int color)
{
	/* TODO: optimize with bit shifts when screen width is 240 ? */
	vram_ptr[(y * SCREEN_W) + x] = color;
}

INLINE void copyPal(int offs, const void *src, int len)
{
	dmaCopy((u32 *)MEM_PAL_BG + offs, src, len);
}

#endif /* LGL_VIDEO_H */
