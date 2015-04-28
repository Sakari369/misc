#include "registers.h"
#include "memory.h"
#include "video.h"
#include "sprites.h"
#include "fonts_common.h"

enum
{
	MAX_SPRITETEXT_LEN = (SCREEN_W / 8) + 2,
};

/* Structure definitions */
typedef struct SpriteChar SpriteChar;
struct SpriteChar
{
	int x;
	int y;
	int index;
	unsigned char ch;
};

typedef struct SpriteText SpriteText;
struct SpriteText
{
	int x;
	int y;
	int w;
	int len;
	int align;
	SpriteChar chars[MAX_SPRITETEXT_LEN];
};

void initSpriteText(SpriteText *t, unsigned char *txt);
void moveSpriteText(SpriteText *text, int x, int y);
