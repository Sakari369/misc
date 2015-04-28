/*
 * bg.h: Background tile stuff.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_BG_H
#define LGL_BG_H

#include "typedefs.h"
#include "memory.h"
#include "registers.h"

#define TILES_PER_ROW	32
#define TILES_PER_COL	20

/* R16_CTRL_BGX definitions */
#define BG_MOSAIC		0x0040
#define BG_PAL256		0x0080
#define BG_WRAP			0x2000

#define BG_MASK_PRIO		0x0003
#define BG_MASK_BASETILE	0x000C
#define BG_MASK_BASEMAP		0x1F00
#define BG_MASK_SIZE		0xC000

#define BG_SHIFT_TILEBASE	2
#define BG_SHIFT_MAPBASE	8
#define BG_SHIFT_MAPSIZE	14

/* Background sizes */
#define BG_SIZE_256x256		0	
#define BG_SIZE_512x256		1
#define BG_SIZE_256x512		2
#define BG_SIZE_512x512		3

/* Background priorities, 3 is the lowest, 0 highest */
#define BG_PRI_0		0
#define BG_PRI_1		1
#define BG_PRI_2		2
#define BG_PRI_3		3

#define BG_MAPADDR(x)		((u16 *)MEM_VIDEO + ((x) << 10))

enum BG_DEFS
{
	MAX_NUM_BGS = 4,
	BG0 = 0,
	BG1,
	BG2,
	BG3
};

/* Structs */

/* Tile structures: Tiles are stored at video memory when using tiled modes.
 * Tiles are stored as tile blocks, 512 8x8 sized tiles or 256 16x16 tiles.
 *
 * There are total number of 6 tile blocks, as there is 96 KB of video memory.
 *
 * Tile blocks 0-3 are reserved for backgrounds, and blocks 4-5 for objects
 * (sprites).
 *
 */

typedef struct Tilemap Tilemap;
struct Tilemap
{
	int w;
	int h;
	const u16 *data;
};

typedef struct BGInfo BGInfo;
struct BGInfo
{
	u16 *map;
	int size;
};

/* External variables */
extern IN_EWRAM BGInfo bgInfo[];

/* Function prototypes */
void setupBg(int bg, int mapbase, int size, int flags);
void copyTilemap(Tilemap *map, int bg);

#endif /* LGL_BG_H */
