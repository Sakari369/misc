/*
 * sound.h: Sound related stuff
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_SOUND_H
#define LGL_SOUND_H

#include "memory.h"
#include "registers.h"

/* R16_CTRL_SOUND_H definitions */
#define SND_OUT_RATIO_25	0x0000
#define SND_OUT_RATIO_50	0x0001
#define SND_OUT_RATIO_100	0x0002
/* Direct channel A definitions */
#define SND_DSA_OUT_RATIO_50	0x0000
#define SND_DSA_OUT_RATIO_100	0x0004
#define SND_DSA_OUT_RIGHT	0x0100
#define SND_DSA_OUT_LEFT	0x0200
#define SND_DSA_OUT_BOTH	0x0300
#define SND_DSA_TIMER0		0x0000
#define SND_DSA_TIMER1		0x0400
#define SND_DSA_FIFO_RESET	0x0800
/* Direct channel B definitions */
#define SND_DSB_OUT_RATIO_50	0x0000
#define SND_DSB_OUT_RATIO_100	0x0008
#define SND_DSB_OUT_RIGHT	0x1000
#define SND_DSB_OUT_LEFT	0x2000
#define SND_DSB_OUT_BOTH	0x3000
#define SND_DSB_TIMER0		0x0000
#define SND_DSB_TIMER1		0x4000
#define SND_DSB_FIFO_RESET	0x8000

#endif /* LGL_SOUND_H */
