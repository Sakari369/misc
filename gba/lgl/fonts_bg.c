#include "fonts_bg.h"

/* Insert one tile at wanted location */
void putBgTile(int bg, int col, int row, int val)
{
	bgInfo[bg].map[(row * CHARS_PER_ROW) + col] = val;
}

/* Clear one row of text */
void clearBgTextRow(int bg, int row)
{
	int i;

	row = row * CHARS_PER_ROW;
	for (i=0; i<CHARS_PER_ROW; i++)
		bgInfo[bg].map[row + i] = CHAR_SPACE;
}

/* Writes string str at wanted location and alignment */
/* Arguments col and row are offsets, so if alignment is specified, the
 * text is moved so much from the alignment position */
void putBgText(int bg, int col, int row, int align, char *str)
{
	int x, y;
	unsigned char *p;

	p = (unsigned char *)str;
	x = 0;
	y = 0;

	/* Figure out the rows and columns from the alignment */
	/* Horizontal alignment */
	if (align & ALIGN_HORI_LEFT)
		x = 0;
	else if (align & ALIGN_HORI_CENTER)
		/* TODO: replace strlen with something else ? */
		//x = (CHARS_PER_ROW / 2) - (strlen(p) / 2) - 1;
		x = (CHARS_PER_ROW / 2) - (strlen(p) / 2);
	else if (align & ALIGN_HORI_RIGHT)
		x = CHARS_PER_ROW - strlen(p);
	/* Vertical alignment */
	if (align & ALIGN_VERT_TOP)
		y = 0;
	else if (align & ALIGN_VERT_CENTER)
		//y = (CHARS_PER_COL / 2) * CHARS_PER_ROW;
		y = 281 + strlen(p);
	else if (align & ALIGN_VERT_BOTTOM)
		y = 30*20 + 8;
	
	/* Add offset if necessary */
	if (row != 0)
		y += row * CHARS_PER_ROW;
	if (col != 0)
		x += col;

	/* Write the text */
	while (*p != '\0')
	{
		switch (*p)
		{
		case '\n':
			y += CHARS_PER_ROW;
			x = col;
			break;
		default: 
			bgInfo[bg].map[y + x] = *p;
			x++;
			break;
		}
		p++;
	}
}
