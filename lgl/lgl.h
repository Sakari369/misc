
/*
 * lgl.h: Includes all LGL headers.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 * I have composed this information from multiple sources, but mostly I have
 * read through the TONC project and composed my own header files for learning
 * purposes. Thanks goes to the author of TONC, <daytshen@hotmail.com>.
 *
 */

#ifndef LGL_H
#define LGL_H

#define LGL_MAJOR_VERSION	0
#define LGL_MINOR_VERSION	3

#include "typedefs.h"
#include "bg.h"
#include "color.h"
#include "debug.h"
#include "dma.h"
#include "fonts_sprite.h"
#include "fonts_bg.h"
#include "input.h"
#include "interrupts.h"
#include "macros.h"
#include "memory.h"
#include "registers.h"
#include "sprites.h"
#include "tiles.h"
#include "timer.h"
#include "video.h"

/* Data */
#include "data_default_font.h"

/* External stuff */
#include "posprintf.h"

#endif /* LGL_H */
