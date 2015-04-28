/*
 * registers.h: GBA register definitions that can be used to control
 * 		something.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 * I have composed this information from multiple sources, but mostly I have
 * read through the TONC project and composed my own header files for learning
 * purposes. Thanks goes to the author of TONC, <daytshen@hotmail.com>.
 *
 */

#ifndef LGL_REGISTERS_H
#define LGL_REGISTERS_H

#include "typedefs.h"
#include "memory.h"

/*
R16_CTRL_DISP, Display control, 16 bits:

Bit(s)	Name	Description
0-2	MODE	Sets video mode, 0-2 tiled, 3-5 bitmap
3	GB	Set if cart is a GBC game. RO
4	DB	If set, bitmap in the doublebuffer will be drawn. Mode 4-5
5	HB	Force processing on HBlank
6	OM	Object mapping mode. 2D (OM=0) or 1D (OM=1)
7	B	Force screen blank.
8	BG0	Enable drawing of bg 0
9	BG1	Enable drawing of bg 1
10	BG2	Enable drawing of bg 2
11	BG3	Enable drawing of bg 3
12	OBJ	Enable sprite drawing
13	WIN0	Enable window 0
14	WIN1	Enable window 1
15	WOBJ	Enable object window
*/
#define R16_CTRL_DISP		*(vu16 *)(MEM_IO + 0x0000)

/*
R16_STAT_DISP, Display status, 16 bits:

Bit(s)	Name	Description
0	VS	Vertical refresh status. Vdraw (VS=0) or vblank (VS=1). RO
1	HS	Horizontal refresh status. Hdraw (VS=0) or hblank (VS=1). RO
2	TS	Trigger status. Set when TI occurs. RO
3	VI	Enable vblank interrupt
4	HI	Enable hblank interrupt
5	TI	Enables trigger interrupt
8-15	T	Vertical line count trigger number
*/
#define R16_STAT_DISP		*(vu16 *)(MEM_IO + 0x0004)

/* R16_VCOUNT, Vertical line counter, 16 bits. 0-159: draw, 160-227 vblank */
#define R16_VCNT		*(vu16 *)(MEM_IO + 0x0006)
// Backwards compat
#define R16_VCOUNT		R16_VCNT

/*
R16_PAD1, Keypad status, 16 bits, bit is 0 if button is down:

Bit(s)	Name	Description
0	A	Button A down
1	B	Button B down
2	SELECT	Button select down
3	START	Button start down
4	RIGHT	Direction pad right down
5	LEFT	Direction pad left down
6	UP	Direction pad up down
7	DOWN	Direction pad down down
8	R	Shoulder button right down
9	L	Shoulder button left down
10-15	-	Unused
*/
#define R16_PAD1		*(vu16 *)(MEM_IO + 0x0130)

/* R16_CTRL_BGX, Background control register

Bit(s)	Name	Description
0-1	PR	Priority
2-3	TB	Base tile-block. (TB=0-3)
6	M	Mosaic flag
7	P	Palette flag. (P=0, 4-bit tiles, 1=8-bit tiles)
8-12	MB	Base map-block. Index of the first mab-block used for bg
13	WR	Wrap flag. Makes affine backgrounds to wrap
14-15	SZ	Background size
*/

#define R16_CTRL_BG0		*(vu16 *)(MEM_BG_CTRL_BASE)
#define R16_HOFF_BG0		*(vu16 *)(MEM_BG_HOFF_BASE)
#define R16_VOFF_BG0		*(vu16 *)(MEM_BG_VOFF_BASE)

#define R16_CTRL_BG1		*(vu16 *)(MEM_BG_CTRL_BASE + 2)
#define R16_HOFF_BG1		*(vu16 *)(MEM_BG_HOFF_BASE + 4)
#define R16_VOFF_BG1		*(vu16 *)(MEM_BG_VOFF_BASE + 4)

#define R16_CTRL_BG2		*(vu16 *)(MEM_BG_CTRL_BASE + 4)
#define R16_HOFF_BG2		*(vu16 *)(MEM_BG_HOFF_BASE + 8)
#define R16_VOFF_BG2		*(vu16 *)(MEM_BG_VOFF_BASE + 8)

#define R16_CTRL_BG3		*(vu16 *)(MEM_BG_CTRL_BASE + 6)
#define R16_HOFF_BG3		*(vu16 *)(MEM_BG_HOFF_BASE + 12)
#define R16_VOFF_BG3		*(vu16 *)(MEM_BG_VOFF_BASE + 12)

/* R32_CTRL_DMAX, DMA control register

Bit(s)	Name	Description
0-15	COUNT	Number of transfers
16-20	-	Unused
21-22	DA	Destination adjustment. DMA_DST_INC, DMA_DST_DEC,
		DMA_DST_FIXED or DMA_DST_RESET
23-24	SA	Source adjustment. DMA_SRC_INC, DMA_SRC_DEC,
		DMA_SRC_FIXED or DMA_SRC_RESET
25	R	Repeats the copy at each Vblank or Hblank if DMA timing set
26	CS	Chunk size in bits, 16 (CS=0) or 32 (CS=1)
27	-	Unused
28-29	TIME	Specifies when the transfer should start. DMA_NOW,
		DMA_AT_VBLANK, DMA_AT_HBLANK or DMA_AT_REFRESH
30	INT	Raise an interrupt when finished
31	EN	Enable the DMA transfer for this channel
*/

/*
There are 4 different DMA channels. Channel 0 has the highest
priority, and 3 the lowest.

Channel	Usage
0	Used for time-critical operations, only with internal RAM
1	Transferring data to the sound buffers for playback		
2	-||-
3	General purpose copies
*/
#define R32_SRC_DMA0		*(vu32 *)(MEM_DMA_SRC_BASE)
#define R32_DST_DMA0		*(vu32 *)(MEM_DMA_DST_BASE)
#define R32_CTRL_DMA0		*(vu32 *)(MEM_DMA_CTRL_BASE)

#define R32_SRC_DMA1		*(vu32 *)(MEM_DMA_SRC_BASE + 12)
#define R32_DST_DMA1		*(vu32 *)(MEM_DMA_DST_BASE + 12)
#define R32_CTRL_DMA1		*(vu32 *)(MEM_DMA_CTRL_BASE + 12)

#define R32_SRC_DMA2		*(vu32 *)(MEM_DMA_SRC_BASE + 24)
#define R32_DST_DMA2		*(vu32 *)(MEM_DMA_DST_BASE + 24)
#define R32_CTRL_DMA2		*(vu32 *)(MEM_DMA_CTRL_BASE + 24)

#define R32_SRC_DMA3		*(vu32 *)(MEM_DMA_SRC_BASE + 36)
#define R32_DST_DMA3		*(vu32 *)(MEM_DMA_DST_BASE + 36)
#define R32_CTRL_DMA3		*(vu32 *)(MEM_DMA_CTRL_BASE + 36)

/* R16_CTRL_TMR*, timer control registers

Bit(s)	Name	Descpription
0-1	F	Frequency in cycles. 0=1, 1=64, 2=256 and 3=1024.
2	C	Cascade. If set, when the counter below overflows, this will
		be incremented. Doesn't work for TMR0.
6	I	Raise interrupt on overflow.
7	E	Enable timer.
*/
#define R16_CTRL_TMR0		*(vu16 *)(MEM_TIMER_CTRL_BASE)
#define R16_CTRL_TMR1		*(vu16 *)(MEM_TIMER_CTRL_BASE + 4)
#define R16_CTRL_TMR2		*(vu16 *)(MEM_TIMER_CTRL_BASE + 8)
#define R16_CTRL_TMR3		*(vu16 *)(MEM_TIMER_CTRL_BASE + 12)

/* R16_DATA_TMRX, timer data registers.

The timer data registers contain 16-bit numbers. You can set the initial value
that the counter will count up from before enabling timers. When the counter
overflows, the counting is started again from this initial value
*/
#define R16_DATA_TMR0		*(vu16 *)(MEM_TIMER_DATA_BASE)
#define R16_DATA_TMR1		*(vu16 *)(MEM_TIMER_DATA_BASE + 4)
#define R16_DATA_TMR2		*(vu16 *)(MEM_TIMER_DATA_BASE + 8)
#define R16_DATA_TMR3		*(vu16 *)(MEM_TIMER_DATA_BASE + 12)

/* Interrupt registers */

/* R32_INTMAIN, points to the interrupt handler */
#define R32_INTMAIN		*(fnptr *)(0x03fffffc)

/* R16_IE, Interrupt enable register. Setting wanted interrupt bits to 1
   enables that interrupt

Bit(s)	Name	Descpription
0	VB	VBlank interrupt.
1	HB	HBlank interrupt.
2	VC	VCount interrupt. High byte of display status register
		contains the vcount at which to raise the interrupt.
3-6	TM	Timer interrupts, 1 bit per timer. Interrupt will be raised
		when the corresponding timer overflows.
7	COM	Serial communication interrupt. Raises when transfer is
		complete (?).
8-11	DMA	DMA interrups, 1 bit per channel. Interrupt will be raised
		when a transfer is complete.
12	K	Keypad interrupt. Raised when any or all of the keys specified
		in the keypad control register are down.
13	C	Cartridge interrupt. Raised when the cart is removed.
*/

#define	R16_IE			*(vu16 *)(MEM_IO + 0x0200)
/* R16_IF, Interrupt request register. When an interrupt is generated, the 
 * corresponding bit here is set to 1. Set the bit again 1 to aknowledge that
 * it has been handled
 */
#define R16_IF			*(vu16 *)(MEM_IO + 0x0202)
/* R16_IME, the Interrupt master enable register. Set to 1 to enable
 * interrupts
 */
#define R16_IME			*(vu16 *)(MEM_IO + 0x0208)

/* Sound registers */

/* R16_CTRL_SND_H, Direct Sound Output control register

Bit(s)	Name	Descpription
0-1	SR	Output sound ratio for channels 1-4. 0=25%, 1=50%, 2=100%
2	RA	Direct sound A output ratio. 0=50%, 1=100%
3	RB	Direct sound B output ratio. -|-.
8	AR	Direct sound A to right output.
9	AL	Direct sound B to left output.
10	AT	Direct sound A sampling rate timer (0 or 1).
11	AFR	Direct sound A FIFO reset.
12	BR	Direct sound B to right output.
13	BL	Direct sound B to left output.
14	BT	Direct sound B sampling rate timer (0 or 1).
15	BFR	Direct sound B FIFO reset.
*/
#define R16_CTRL_SND_H		*(vu16 *)(MEM_IO + 0x0082)

/* R16_CTRL_SND_X, Master sound output control / status.

Bit(s)	Name	Descpription
0	FMS1	FM Sound 1 status. These are read-only.
1	FMS2	FM Sound 2 status.
2	FMS3	FM Sound 3 status.
3	FMS4	FM Sound 4 status.
7	ASE	All sound enable. 0=off, 1=on.
*/
#define R16_CTRL_SND_X		*(vu16 *)(MEM_IO + 0x0084)

/* R16_CTRL_SNDBIAS, Sound bias control

Bit(s)	Name	Descpription
0-9	DCOff	DC offset bias value. Should not be changed!
14-15	PWMRes	PWM resampling resolution:
		0 = 9 bit at 32768 Hz (default)
		1 = 8 bit at 65536 Hz (most, if not all games use this)
		2 = 7 bit at 131072 Hz
		3 = 6 bit at 262144 Hz
*/
#define R16_CTRL_SNDBIAS	*(vu16 *)(MEM_IO + 0x0088)

/* R16_FIFO_X, sound buffers for both direct sound channels. 
 * 8-bit samples are be placed here. */
#define R16_FIFO_A		*(vu16 *)(MEM_IO + 0x00A0)
#define R16_FIFO_B		*(vu16 *)(MEM_IO + 0x00A4)

#endif /* LGL_REGISTERS_H */
