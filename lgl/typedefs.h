/*
 * typedefs.h: Type definitions used with GBA development.
 *
 * Copyright (C) 2004 Sakari Lehtonen <sakari@ionstream.fi>
 *
 */

#ifndef LGL_TYPEDEFS_H
#define LGL_TYPEDEFS_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned int vu32;

typedef volatile signed char vs8;
typedef volatile signed short vs16;
typedef volatile signed int vs32;

/* Fixed point number, just to clarify that the variable is 
 * using fixed point numbers */
typedef int fixed;

/* Function pointer void func(void) */
typedef void (*fnptr)(void);

#if 0
#ifndef bool
typedef enum bool bool;
enum bool
{
	false = 0,
	true = 1
};
#endif
#endif

#ifndef INLINE
  #define INLINE static inline
#endif

#endif /* LGL_TYPEDEFS_H */
