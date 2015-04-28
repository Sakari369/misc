/*
 * timer.h: Timer definitions and related stuff.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_TIMER_H
#define LGL_TIMER_H

#include "typedefs.h"
#include "registers.h"

#define TMR_CASCADE		0x0004
#define TMR_INTERRUPT		0x0040
#define TMR_ENABLE		0x0080

#define TMR_FREQ_1		0x0000
#define TMR_FREQ_64		0x0001
#define TMR_FREQ_256		0x0002
#define TMR_FREQ_1024		0x0003

#define TMR_MASK_FREQ		0x0003

#endif /* LGL_TIMER_H */
