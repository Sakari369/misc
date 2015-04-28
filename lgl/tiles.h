#ifndef LGL_TILES_H
#define LGL_TILES_H

#include "typedefs.h"
#include "memory.h"
#include "registers.h"
#include "video.h"

/* 4 bpp Tile, 8x8x4 = 256 bits (8x32)  */
typedef struct Tile4bpp Tile4bpp;
struct Tile4bpp
{
	u32 data[8];
};

/* 4 bpp TileBlock, 64 tiles */
typedef Tile4bpp TileBlock4bpp[512];

/* 8 bpp Tile, 8x8x8 = 512 bits (16x32)  */
typedef struct Tile8bpp Tile8bpp;
struct Tile8bpp
{
	u32 data[16];
};

/* 8 bpp TileBlock, 32 tiles */
typedef Tile8bpp TileBlock8bpp[256];

/* These are used to choose the correct tileblock. There is one for every
 * background layer and one for the sprite */
enum TILEBLOCK_DEFS
{
	TB_BG0 = 0,
	TB_BG1,
	TB_BG2,
	TB_BG3,
	TB_OBJ
};

/* Memory pointers, 4bpp and 8bpp */
#define TileMem4	((TileBlock4bpp *)MEM_VIDEO)
#define TileMem8	((TileBlock8bpp *)MEM_VIDEO)

/* Inline functions */

INLINE void copyTiles8(int block, int tile, const void *src, int len)
{
	dmaCopy(&TileMem8[block][tile], src, len);
}

INLINE void copyTiles4(int block, int tile, const void *src, int len)
{
	dmaCopy(&TileMem4[block][tile], src, len);
}

#endif /* LGL_TILES_H */
