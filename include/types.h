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

#endif /* TYPES_H */
