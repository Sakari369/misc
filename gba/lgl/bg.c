#include "bg.h"

IN_EWRAM BGInfo bgInfo[MAX_NUM_BGS];

/* Setups the background bg: Sets the size of the tilemap, base of the
 * character tile map and various flags (priority etc) */
void setupBg(int bg, int mapbase, int size, int flags)
{
	int temp;

	/* base is the starting address of character tile map */
	temp = (flags | (size << BG_SHIFT_MAPSIZE) | \
		(mapbase << BG_SHIFT_MAPBASE));

	/* Store the initialized values */
	bgInfo[bg].map = BG_MAPADDR(mapbase);
	bgInfo[bg].size = size;

	switch (bg)
	{
	case BG0:
		R16_CTRL_BG0 = temp;
		break;
	case BG1:
		R16_CTRL_BG1 = temp;
		break;
	case BG2:
		R16_CTRL_BG2 = temp;
		break;
	case BG3:
		R16_CTRL_BG3 = temp;
		break;
	}
}

/* Copies tilemap map to memory */
void copyTilemap(Tilemap *map, int bg)
{
	int x, y;

	/* TODO: handle the base thingie ? */
	for (y=0; y < map->h; y++)
		for (x=0; x < map->w; x++)
			bgInfo[bg].map[(y * map->h) + x] = \
				map->data[(y * map->h) + x];
}
