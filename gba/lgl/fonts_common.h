#ifndef LGL_FONTS_COMMON_H
#define LGL_FONTS_COMMON_H

#include "registers.h"
#include "memory.h"
#include "video.h"
#include "bg.h"

#define CHAR_SPACE	' '

enum FONTS_DEFS
{
	FONT_WIDTH = 8,
	ASCII_START = 32,
	/* XXX this was at some point 30, not 32 as TILES_PER_ROW */
	CHARS_PER_ROW = TILES_PER_ROW,
	CHARS_PER_COL = TILES_PER_COL,
};
	
enum FONTS_ALIGNMENT
{
	ALIGN_VERT_TOP 		= (1 << 0),
	ALIGN_VERT_CENTER	= (1 << 1),
	ALIGN_VERT_BOTTOM 	= (1 << 2),
	ALIGN_HORI_LEFT 	= (1 << 3),
	ALIGN_HORI_CENTER 	= (1 << 4),
	ALIGN_HORI_RIGHT 	= (1 << 5)
};

#endif /* LGL_FONTS_COMMON_H */
