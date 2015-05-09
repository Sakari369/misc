/*
 * macros.h: My GBA macros.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_MACROS_H
#define LGL_MACROS_H

#include "typedefs.h"

/* Boundary checking macros */
#define MAX(a, b)		(((a) > (b)) ? (a) : (b))
#define MIN(a, b)		(((a) < (b)) ? (a) : (b))
#define CLAMP(x, low, high)	(((x)>(high))?(high):(((x)<(low))?(low):(x)))
#define	ABS(a)			(((a) < 0) ? -(a) : (a))

/* Misc macros */
#define NELEMS(a)		(int)(sizeof(a)/sizeof(a[0]))

/* Fixed point math macros. Information gathered from the document
 * "Fixed point arithmetic on the ARM" by Advanced RISC machines Ltd */

/* The default value for fixed number shifts */
#define FSH	16

/* Basic operations to add fixed point numbers. Used to clarify that the
 * added numbers are fixed point numbers */
#define FADD(a, b)		((a) + (b))
#define FSUB(a, b)		((a) - (b))

/* Basic operations where a is of fixed point q format and b is an integer */
#define FADDI(a, b, q)		((a) + ((b) << (q)))
#define FSUBI(a, b, q)		((a) - ((b) << (q)))

/* Convert from q1 format to q2 format */
#define FCONV(a, q1, q2)	(((q2)>(q1)?(a)<<((q2)-(q1)):(a)>>((q1)-q2)))

/* Operatons between a in q1 format and b in q2 format returning the result
 * in q3 format */
#define FADDG(a, b, q1, q2, q3)	(FCONV(a,q1,q3) + FCONV(b,q2,q3))
#define FSUBG(a, b, q1, q2, q3) (FCONV(a,q1,q3) - FCONV(b,q2,q3))

/* Convert from integer to fixed point and back */
#define TOFIX(a, q)		((a) << (q))
#define TOINT(a, q)		((a) >> (q))

#endif /* LGL_MACROS_H */
