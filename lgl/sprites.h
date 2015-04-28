/*
 * sprites.h: Sprite (or OAM) stuff.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 * I have composed this information from multiple sources, but mostly I have
 * read through the TONC project and composed my own header files for learning
 * purposes. Thanks goes to the author of TONC, <daytshen@hotmail.com>.
 *
 */

#ifndef LGL_SPRITES_H
#define LGL_SPRITES_H

#include "typedefs.h"
#include "debug.h"
#include "memory.h"
#include "registers.h"
#include "tiles.h"
#include "video.h"

//#define USE_SPRITE_BUF

/* Maximum number of sprites (objects). The GBA has 128 hardware sprites */
#define	MAX_NUM_SPRITES	128

/* Pointers to Object Attribute Memory */
#define OAM_PTR		((u16 *)MEM_OAM)	
#define OAM_DATA_PTR	((u16 *)(MEM_VIDEO + 0x10000))

/* Object Attribute 0, 16 bits:
 *
 * Bit(s)	Name	Description
 * 0-7		Y	Y coordinate, top of sprite
 * 8		AFF	Transformable flag
 * 9		DS	Double Size flag
 * 10		BL	Alpha Blending flag
 * 11		W	Blending with window
 * 12		M	Mosaic flag
 * 13		P	Palette flag. 16-color (P=0) or 256-color (P=1)
 * 14-15	SH	Shape. Square (SH=0), wide (SH=1) or tall (SH=2)
 *
 */

#define	OAM_A0_AFFINE		0x0100
#define OAM_A0_DOUBLESIZE	0x0200
#define OAM_A0_BLEND		0x0400
#define OAM_A0_WINBLEND		0x0800
#define OAM_A0_MOSAIC		0x1000
#define OAM_A0_PAL256		0x2000
#define OAM_A0_SQUARE		0
#define OAM_A0_WIDE		0x4000
#define OAM_A0_TALL		0x8000

#define SHAPE_SQUARE		OAM_A0_SQUARE
#define SHAPE_TALL		OAM_A0_TALL
#define SHAPE_WIDE		OAM_A0_WIDE

#define OAM_A0_MASK_Y		0x00FF
#define OAM_A0_MASK_SHAPE	0xC000

/* Object Attribute 1, 16 bits
 *
 * Bit(s)	Name	Description
 * 0-8		X	X coordinate, left of sprite
 * 9-11		R	If AFF flag in attr0 is set, index of ObjectionAffine
 * 12		H	If AFF flag clear, flips the sprite horizontally
 * 13		V	If AFF flag clear, flips the sprite vertically
 * 14-15	SZ	Size
 *
 */
#define OAM_A1_HFLIP		0x1000
#define OAM_A1_VFLIP		0x2000

#define OAM_A1_MASK_X		0x01FF
#define OAM_A1_MASK_AFFINDEX	0x0700
#define OAM_A1_MASK_SIZE	0xC000

#define OAM_A1_SIZE_8		0x0000
#define OAM_A1_SIZE_16		0x4000
#define OAM_A1_SIZE_32		0x8000
#define OAM_A1_SIZE_64		0xC000

#define SIZE_8			OAM_A1_SIZE_8	
#define SIZE_16			OAM_A1_SIZE_16	
#define SIZE_32			OAM_A1_SIZE_32
#define SIZE_64			OAM_A1_SIZE_64

/* Object Attribute 2, 16 bits
 *
 * Bit(s)	Name	Description
 * 0-9		TL	Base tile-index of sprite.
 * 10-11	PR	Priority.
 * 12-15	SP	Sub palette to use in 16-color mode.
 *
 */

#define OAM_A2_MASK_TILEINDEX	0x03FF
#define OAM_A2_MASK_PRI		0x0C00
#define OAM_A2_MASK_PAL		0xF000

#define OAM_A2_SHIFT_PRI	10

#define OAM_A2_PRI_0		0
#define OAM_A2_PRI_1		0x0400
#define OAM_A2_PRI_2		0x0800
#define OAM_A2_PRI_3		0x0C00

#define PRI_0			OAM_A2_PRI_0		
#define PRI_1			OAM_A2_PRI_1
#define PRI_2			OAM_A2_PRI_2		
#define PRI_3			OAM_A2_PRI_3

#define OAM_A2_PAL_0		0x0000
#define OAM_A2_PAL_1		0x1000
#define OAM_A2_PAL_2		0x2000
#define OAM_A2_PAL_3		0x3000
#define OAM_A2_PAL_4		0x4000
#define OAM_A2_PAL_5		0x5000
#define OAM_A2_PAL_6		0x6000
#define OAM_A2_PAL_7		0x7000
#define OAM_A2_PAL_8		0x8000
#define OAM_A2_PAL_9		0x9000
#define OAM_A2_PAL_10		0xA000
#define OAM_A2_PAL_11		0xB000
#define OAM_A2_PAL_12		0xC000
#define OAM_A2_PAL_13		0xD000
#define OAM_A2_PAL_14		0xE000
#define OAM_A2_PAL_15		0xF000

#define PAL_0			OAM_A2_PAL_0	
#define PAL_1			OAM_A2_PAL_1	
#define PAL_2			OAM_A2_PAL_2	
#define PAL_3			OAM_A2_PAL_3	
#define PAL_4			OAM_A2_PAL_4	
#define PAL_5			OAM_A2_PAL_5	
#define PAL_6			OAM_A2_PAL_6	
#define PAL_7			OAM_A2_PAL_7	
#define PAL_8			OAM_A2_PAL_8	
#define PAL_9			OAM_A2_PAL_9	
#define PAL_10			OAM_A2_PAL_10
#define PAL_11			OAM_A2_PAL_11
#define PAL_12			OAM_A2_PAL_12
#define PAL_13			OAM_A2_PAL_13
#define PAL_14			OAM_A2_PAL_14
#define PAL_15			OAM_A2_PAL_15

/* Structure that contains information about sprites. Fill is just
 * there to align it correctly in memory */
typedef struct SpriteInfo SpriteInfo;
struct SpriteInfo
{
	u16 attr0;
	u16 attr1;
	u16 attr2;
	s16 fill;
};

/* Structure that holds affine transformation information for sprites */
typedef struct SpriteAffine SpriteAffine;
struct SpriteAffine
{
	u16 fill0[3];
	s16 pa;
	u16 fill1[3];
	s16 pb;
	u16 fill2[3];
	s16 pc;
	u16 fill3[3];
	s16 pd;
};

/* This might not be the Right Way To Do Things, but it works and is
 * faster when compared to using a global pointer to a memory address that
 * will never change during the execution of a program */
#ifndef USE_SPRITE_BUF
  #define	sprites ((SpriteInfo *)(MEM_OAM))
#else
  extern IN_EWRAM SpriteInfo spriteBuffer[MAX_NUM_SPRITES];
  #define	sprites spriteBuffer
  #define	sprite_info ((SpriteInfo *)(MEM_OAM))
#endif

/* External data */
extern IN_EWRAM int spriteCount;
extern IN_EWRAM int tileIndex;
extern IN_EWRAM u16 spriteTileIndexes[];
extern IN_EWRAM int spriteIndex;
extern IN_EWRAM int spriteDataIndex;

/* Function prototypes */
void initSprites(void);
void hideSprites(void);
void setSpritePos(int index, int x, int y);
void setSpritePal(int oamIndex, int pal);
void setSpritePri(int oamIndex, int pri);
void copySpriteTiles(void *src, int len, int id);
#ifdef USE_SPRITE_BUF
  void updateSprites(int count);
#endif

/* Inline functions */
INLINE void copySpritePal(int index, const u16 *src, int len)
{
	dmaCopy(PAL_OBJ_PTR + (index * 16), src, len);
}

INLINE void hideSprite(int index)
{
	//setSpritePos(index, SCREEN_W, SCREEN_H);
	sprites[index].attr0 |= OAM_A0_DOUBLESIZE;
}

INLINE void showSprite(int index)
{
	sprites[index].attr0 &= ~OAM_A0_DOUBLESIZE;
}

INLINE int getSpritePosX(int index)
{
	return (sprites[index].attr1 & OAM_A1_MASK_X);
}

INLINE int getSpritePosY(int index)
{
	return (sprites[index].attr0 & OAM_A0_MASK_Y);
}

INLINE void setSpriteAttr(int index, u16 attr0, u16 attr1, u16 attr2)
{
	sprites[index].attr0 = attr0;
	sprites[index].attr1 = attr1;
	sprites[index].attr2 = attr2;
}

INLINE void setSpriteHFlip(int index)
{
	sprites[index].attr1 |= OAM_A1_HFLIP;
}

INLINE void resetSpriteHFlip(int index)
{
	sprites[index].attr1 &= ~OAM_A1_HFLIP;
}

#if 0
/* Inline version of the setSpritePos .. faster when used with loops */
INLINE void _setSpritePos(int index, int x, int y)
{
	u16 temp;
	
	/* Mask out the y and set the new value */
	temp = sprites[index].attr0;
	temp &= ~OAM_A0_MASK_Y;
	temp |= (y & OAM_A0_MASK_Y);
	sprites[index].attr0 = temp;
	
	/* Mask out the x and set the new value */
	temp = sprites[index].attr1;
	temp &= ~OAM_A1_MASK_X;
	temp |= (x & OAM_A1_MASK_X);
	sprites[index].attr1 = temp;
}
#endif

#endif /* LGL_SPRITES_H */
