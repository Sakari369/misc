/*
 * fonts_bg.h: Font routines that draw on the background layers
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_FONTS_BG_H
#define LGL_FONTS_BG_H

#include <string.h>
#include "memory.h"
#include "registers.h"
#include "video.h"
#include "bg.h"
#include "fonts_common.h"

/*
typedef struct BgTile BgTile;
struct BgTile
{
	char ch;
	int x;
	int y;
};
*/

typedef struct BgText BgText;
struct BgText
{
	char *str;
	int col;
	int row;
	int align;
};

/* Inline functions */

/* Clears the whole text background */
INLINE void clearBgText(int bg)
{
	memSet(bgInfo[bg].map, CHAR_SPACE, CHARS_PER_COL*CHARS_PER_ROW);
}

INLINE int getBgTile(int bg, int col, int row)
{
	return bgInfo[bg].map[(row * CHARS_PER_ROW) + col];
}

/* Function prototypes */
void putBgTile(int bg, int col, int row, int val);
void putBgText(int bg, int col, int row, int align, char *str);
void clearBgText(int bg);

#endif /* LGL_FONTS_BG_H */
