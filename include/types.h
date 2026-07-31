#ifndef TYPES_H
#define TYPES_H

/* Fixed-width integer types used throughout the decomp.
   Previously re-declared locally in 1106 src/ files. */
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed char    s8;
typedef signed short   s16;
typedef signed int     s32;
typedef unsigned long long u64;
typedef signed long long   s64;

/* 20.12 fixed-point scalar, as used by the SDK/game maths.
   Fix12i is the spelling used by most existing src/ files; both are the same type. */
typedef s32 Fix12;
typedef s32 Fix12i;

typedef struct Vector3 {
    Fix12i x, y, z;
} Vector3;

typedef struct Vector3s {
    s16 x, y, z;
} Vector3s;


/* Floating-point aliases. */
typedef float  f32;
typedef double f64;

/* Volatile aliases for memory-mapped hardware registers. */
typedef volatile unsigned char  vu8;
typedef volatile signed char    vs8;
typedef volatile unsigned short vu16;
typedef volatile signed short   vs16;
typedef volatile unsigned int   vu32;
typedef volatile signed int     vs32;

/* Small enum: how an actor responds to being eaten by Yoshi. */
typedef int OnYoshiEatReturnVal;

#ifndef __cplusplus
typedef int bool;  /* a keyword in C++, an ordinary identifier in C */
#endif

#endif /* TYPES_H */
