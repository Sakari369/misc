/*
 * input.h: GBA input stuff.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_INPUT_H
#define LGL_INPUT_H

#include "typedefs.h"
#include "registers.h"

/* R16_KEYPAD definitions */
/* Keys */
#define KEY_A		0x0001
#define KEY_B		0x0002
#define KEY_SELECT	0x0004
#define KEY_START	0x0008
#define KEY_RIGHT	0x0010
#define KEY_LEFT	0x0020
#define KEY_UP		0x0040
#define KEY_DOWN	0x0080
#define KEY_R		0x0100
#define KEY_L		0x0200

/* Any key */
#define KEY_ANY		0x03FF
/* Any of the direction pad keys */
#define KEY_DPAD	0x00F0
/* Any of the should keys */
#define KEY_SHOULDER	0x0300

/* Start + select + a + b */
#define KEY_RESET	0x000F

/* Function prototypes */

void pollKeys(void);
int keyDown(const int key);
int keyUp(const int key);
int keyWasDown(const int key);
int keyWasUp(const int key);
int keyHit(const int key);

/* Extern variables */


#if 0
extern u16 curr_input;
extern u16 prev_input;

/* Function prototypes */
void pollKeys(void);

/* Inline functions */

/* Check if key is down */
INLINE u16 keyDown(const int key)
{
	return (~(curr_input) & key);
}

/* Check if key is up */
INLINE u16 keyUp(const int key)
{
	return (curr_input & key);
}

INLINE u16 keyWasDown(const int key)
{
	return (~(prev_input) & key);
}

/* Check if key is up */
INLINE u16 keyWasUp(const int key)
{
	return (prev_input & key);
}

INLINE u16 keyHit(const int key)
{
	return ((~curr_input & prev_input) & key);
}
#endif

#endif /* LGL_INPUT_H */
