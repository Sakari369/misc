#include "input.h"

static int curr_input = KEY_ANY;
static int prev_input = KEY_ANY;

void pollKeys(void)
{
	prev_input = curr_input;
	curr_input = R16_PAD1;
}

int keyDown(const int key)
{
	return (~(curr_input) & key);
}

/* Check if key is up */
int keyUp(const int key)
{
	return (curr_input & key);
}

int keyWasDown(const int key)
{
	return (~(prev_input) & key);
}

/* Check if key is up */
int keyWasUp(const int key)
{
	return (prev_input & key);
}

int keyHit(const int key)
{
	return ((~curr_input & prev_input) & key);
}
