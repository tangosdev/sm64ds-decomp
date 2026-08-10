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

   NOT named `Fix12`. In the original C++ that is a class TEMPLATE, not a scalar typedef --
   the ROM's own mangled symbols spell it out, e.g. `_ZN4cstd5atan2E5Fix12IiES1_` demangles
   to `cstd::atan2(Fix12<int>, Fix12<int>)`. Taking the name here for a plain `s32` would
   collide with the real type the moment it is reconstructed, and every consumer would have
   to be edited and re-verified to give it back. `Fix12i` is already the spelling most src/
   files use and is not a real C++ type name, so it stays free.

   A local `typedef s32 Fix12;` in an individual src/ file is still fine -- it is private to
   that translation unit. This rule is about what the SHARED header claims. */
typedef s32 Fix12i;

typedef struct Vector3 {
    Fix12i x, y, z;
#ifdef __cplusplus
    /* DECLARED, AND EMPTY, BECAUSE THE ROM DESTROYS ARRAYS OF IT. A POD array
       needs no cleanup, so ChiefChilly's
       `__destroy_arr(this + 0x3e8, 8, 0xc, func_020072c0)` can only exist if
       the element type declares a destructor -- and 0x020072c0 is four bytes,
       `bx lr`, an empty one. config/arm9/symbols.txt names it
       _ZN7Vector3D1Ev.

       This makes Vector3 non-POD everywhere, which is nearly free but not
       quite: 10,812 of 10,813 enrolled functions are byte-identical either way,
       and Rabbit::Behavior grows 8 bytes (0x5cc -> 0x5d4). That one is the
       price of the six destructors this unblocks. */
    ~Vector3() {}
#endif
} Vector3;

#ifdef __cplusplus
/* 0xc, and the ROM agrees twice over: ChiefChilly's arrays stride by 0xc and
   its __destroy_arr calls pass 0xc as the element size. */
typedef char Vector3_size_must_be_0xc[sizeof(Vector3) == 0xc ? 1 : -1];
#endif

typedef struct Vector3s {
    s16 x, y, z;
#ifdef __cplusplus
    /* Declared and empty, for the same reason Vector3's is: Wiggler destroys an
       array of a 6-byte type through
       __destroy_arr(this + 0x444, 5, 6, func_02011508), a POD array needs no
       cleanup, and 0x02011508 is four bytes of `bx lr`. 6 is sizeof(Vector3s). */
    ~Vector3s() {}
#endif
} Vector3s;

#ifdef __cplusplus
typedef char Vector3s_size_must_be_0x6[sizeof(Vector3s) == 6 ? 1 : -1];
#endif


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
