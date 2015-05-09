#include "fonts_sprite.h"

/* Initializes spriteText with string str
 * TODO: fix subpalette thing */
void initSpriteText(SpriteText *t, unsigned char *str)
{
	int i;
	int attr2;
	int xoff;
	unsigned char *ptr;

	/* Pointer to the actual string */
	ptr = str;

	t->len = 0;
	i = 0;
	xoff = 0;
	while (*ptr != '\0' && i < MAX_SPRITETEXT_LEN)
	{
		/* Set up the individual character */
		t->chars[i].x = t->x + xoff;
		t->chars[i].y = t->y;
		t->chars[i].ch = *ptr;
		t->chars[i].index = spriteIndex;
		
		/* Set up the character sprite */
		attr2 = str[i];
		setSpriteAttr(spriteIndex, OAM_A0_SQUARE, OAM_A1_SIZE_8,
			attr2);
		setSpritePos(spriteIndex, t->chars[i].x, t->chars[i].y);
		spriteIndex++;

		/* Increase */
		ptr++;
		i++;
		xoff += FONT_WIDTH;
	}
	
	/* Length and width of spriteText */
	t->len = i;
	t->w = xoff;
}

/* Move spriteText location on screen */
void moveSpriteText(SpriteText *t, int x, int y)
{
	int i;
	int xoff;

	xoff = 0;
	/* Move all the characters */
	for (i=0; i<t->len; i++)
	{
		t->chars[i].x = x + xoff;
		t->chars[i].y = y;

		setSpritePos(t->chars[i].index, 
			t->chars[i].x, t->chars[i].y);
		
		xoff += FONT_WIDTH;
	}
}

