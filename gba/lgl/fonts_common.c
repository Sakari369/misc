#include "fonts_common.h"

/*
IN_EWRAM Fontset loadedFontsets[MAX_FONTSETS];
IN_EWRAM u32 fontsetCount = 0;
*/

void initFontset(Fontset *s)
{
	/*
	if (fontsetCount > MAX_FONTSETS)
		return;
	*/

	/* Start the fontset at tilemem 32, so we can put ASCII
	 * characters straight into the tilemaps */
	dmaCopy(&TileMem4[TB_BG0][32], s->spriteData, s->size);
}
