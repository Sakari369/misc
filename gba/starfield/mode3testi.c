#include "lgl.h"
#include <stdio.h>
#include <stdlib.h>

enum
{
	NUM_STARS = 128,
	ORIGO_X = SCREEN_W / 2,
	ORIGO_Y = SCREEN_H / 2,
	MAXX = SCREEN_W,
	MINX = 0,
	MAXY = SCREEN_H,
	MINY = 1,
	STAR_SPEED = 4
};

enum
{
	COLOR_DARKEST = RGB(14, 14, 10),
	COLOR_DARK = RGB(18, 18, 20),
	COLOR_MEDIUM = RGB(22, 22, 24),
	COLOR_BRIGHT = RGB(29, 29, 30),
	COLOR_BRIGHTEST = RGB(30, 30, 31)
};

#define starposx(i) (((star[i].x << 7) / star[i].z) + ORIGO_X)
#define starposy(i) (((star[i].y << 7) / star[i].z) + ORIGO_Y)

typedef struct STAR STAR;
struct STAR
{
	short int x;
	short int y;
	short int z;
};

IN_EWRAM STAR star[NUM_STARS];

IN_EWRAM short int randXtable[NUM_STARS]; 
IN_EWRAM short int randYtable[NUM_STARS];

void initStars(void)
{
	int i;

	for (i=0; i<NUM_STARS; i++)
	{
		randXtable[i] = rand()%MAXX - ORIGO_X;
		randYtable[i] = rand()%MAXY - ORIGO_Y;
		star[i].x = randXtable[i];
		star[i].y = randYtable[i];
		star[i].z = rand()%256 + 1;
	}
}

void drawStars(void)
{
	int x, y, i;
	int color= RGB(0, 0, 31);

	for (i=0; i<NUM_STARS; i++)
	{
		x = starposx(i);
		y = starposy(i);
		
		if (x > MINX && x < MAXX && y > MINY && y < MAXY) 
		{
			color = RGB(0, 0, 0);
			putPixel(x, y, color);
		}

		star[i].z -= STAR_SPEED;
		if (star[i].z <= 0)
		{
			star[i].x = randXtable[i];
			star[i].y = randYtable[i];
			star[i].z = 256;
		}

		x = starposx(i);
		y = starposy(i);

		if (x > MINX && x < MAXX && y > MINY && y < MAXY)
		{
			if (star[i].z > 224)
				color = COLOR_DARKEST;
			else if (star[i].z > 192)
				color = COLOR_DARK;
			else if (star[i].z > 128)
				color = COLOR_MEDIUM;
			else if (star[i].z > 96)
				color = COLOR_BRIGHT;
			else if (star[i].z > 16)
				color = COLOR_BRIGHTEST;
			putPixel(x, y, color);
		}
	}
}

int main(void)
{
	setVideoMode(VID_MODE3 | VID_ENABLE_BG2 | VID_LINEAR);

	initStars();

	while (1)
	{
		drawStars();
		waitVsync();
	}

	return 0;
}
