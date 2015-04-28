#include "sprites.h"

/* Sprite system:
 *
 * Every sprite has an unique id that we identify that sprite with.
 *
 * We keep track of those id's, and get the actual sprite number by that
 * id.
 */

#ifdef USE_SPRITE_BUF
  IN_EWRAM SpriteInfo spriteBuffer[MAX_NUM_SPRITES];
#endif

/* The current sprite count */
IN_EWRAM int spriteCount = 0;
/* The current tileIndex */
IN_EWRAM int tileIndex = 0;
/* spriteTileIndexes holds the tile indexes for different sprite datas */
IN_EWRAM u16 spriteTileIndexes[MAX_NUM_SPRITES];
/* Sprite index holds the next free slot in OAM memory */
IN_EWRAM int spriteIndex = 0;
/* spritedataIndex holds to the next free slot in the sprite data memory */
IN_EWRAM int spriteDataIndex = 0;

/* Helper function to copy sprite tiles and update indexing information
 * Used when copying game objects in a row .. */
void copySpriteTiles(void *src, int len, int id)
{
	spriteCount++;
	dmaCopy(&TileMem4[TB_OBJ][spriteDataIndex], (u32 *)src, len);
	spriteDataIndex += len / 8;
	spriteTileIndexes[id] = tileIndex;
	tileIndex += len / 8;
}

/* Initialize all sprites to off-screen */
void initSprites(void)
{
	int i;

	for (i=0; i<MAX_NUM_SPRITES; i++)
	{
		sprites[i].attr0 = SCREEN_H;
		sprites[i].attr1 = SCREEN_W;
		spriteTileIndexes[i] = 0;
	}
#ifdef USE_SPRITE_BUF
	updateSprites(MAX_NUM_SPRITES);
#endif
}

/* Hide all sprites that have been initialized */
void hideSprites(void)
{
	int i;

	for (i=0; i<spriteCount; i++)
		setSpritePos(i, SCREEN_W, SCREEN_H);	
}

/*
void setMaskVal(int oamIndex, int orig, int mask, int new)
{
	orig &= ~mask;
	orig |= (new & mask);
	return orig;
}
*/

void setSpritePal(int oamIndex, int pal)
{
	u16 temp;

	temp = sprites[oamIndex].attr2;
	temp &= ~OAM_A2_MASK_PAL;
	temp |= (pal & OAM_A2_MASK_PAL);
	sprites[oamIndex].attr2 = temp;
}

void setSpritePri(int oamIndex, int pri)
{
	u16 temp;

	temp = sprites[oamIndex].attr2;
	temp &= ~OAM_A2_MASK_PRI;
	temp |= (pri & OAM_A2_MASK_PRI);
	sprites[oamIndex].attr2 = temp;
}

/* Set the screen co-ordinates of object in buffer index to point p */
void setSpritePos(int index, int x, int y)
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

	/*
	sprites[index].attr0 &= ~OAM_A0_MASK_Y;
	sprites[index].attr0 |= (y & OAM_A0_MASK_Y);
	sprites[index].attr1 &= ~OAM_A1_MASK_X;
	sprites[index].attr1 |= (x & OAM_A1_MASK_X);
	*/
}

/* use DMA ? */
#ifdef USE_SPRITE_BUF
void updateSprites(int count)
{
	int i;

	for (i=0; i<count; i++)
	{
		sprite_info[i].attr0 = sprites[i].attr0;
		sprite_info[i].attr1 = sprites[i].attr1;
		sprite_info[i].attr2 = sprites[i].attr2;
	}
}
#endif
