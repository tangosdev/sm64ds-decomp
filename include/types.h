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

/* size_t, which C++ requires as the first parameter of every `operator new`.
   The two compilers that build this tree spell it differently and each refuses
   the other's spelling, because the rule is on the TYPE and not on the width --
   all three candidates are four bytes here. mwccarm rejects
   `operator new(unsigned int)` with "illegal 'operator' declaration"; 32-bit
   MSVC rejects `operator new(unsigned long)` with C2821, "first formal
   parameter to 'operator new' must be 'size_t'". Neither arm below is visible
   to the other compiler, so no ROM byte moves: mwccarm keeps the `unsigned
   long` the nine actor headers already declared. */
#ifdef _MSC_VER
#include <stddef.h>          /* the host's own size_t, so the host rule holds by definition */
#else
typedef unsigned long size_t;
#endif

/* Pointer-sized signed integer: an address kept in an integer. The DS build is
   32-bit only, so mwccarm spells an address `int`; a 64-bit host needs the
   wider type, which the host's own header owns. Casts through it are byte-free
   on the DS build (`intptr_t` and `int` are then the same type) and stay
   correct wherever a pointer no longer fits an `int`. */
#ifdef _MSC_VER
#include <stdint.h>
#else
typedef signed int intptr_t;
#endif

/* NitroSDK's fixed-point scalars (nitro/fx/fx.h): fx32 is 20.12 in an s32,
   fx16 is 4.12 in an s16. Declared here under the same guard as fx.h so every
   file sees the SDK's spelling without pulling in fx.h's vectors and macros,
   and a file may include both headers in either order.

   fx32 is the only spelling of a 20.12 scalar in this tree. The ROM is
   stripped: it carries RTTI class names and no function symbols, so no type
   spelling of a 20.12 value comes from the cartridge. What the toolchain
   verifiably had is the SDK's fx32 (the NitroSDK functions linked into this
   ROM take it), so that is the period spelling, with FX32_SHIFT and FX_MUL
   from nitro/fx/fx.h. An earlier community name, Fix12i, was retired for it;
   both named `signed int`, so the respelling changed no byte and no mangled
   name. Fix12<T> (include/math/Fix12.h) is a different thing, a class
   template whose by-value parameters change calling convention
   (notes/mwccarm-codegen.md 6az): converting to it is matching work. */
#ifndef NITRO_FX_SCALAR_TYPES
#define NITRO_FX_SCALAR_TYPES
typedef signed int fx32;
typedef signed long long fx64;
typedef signed long long fx64c;
typedef signed short fx16;
#endif

typedef struct Vector3 {
    fx32 x, y, z;
#ifdef __cplusplus
    /* DECLARED, AND EMPTY, BECAUSE THE ROM DESTROYS ARRAYS OF IT. A POD array
       needs no cleanup, so daKing_Donketu_c's
       `__cxa_vec_cleanup(this + 0x3e8, 8, 0xc, func_020072c0)` can only exist if
       the element type declares a destructor -- and 0x020072c0 is four bytes,
       `bx lr`, an empty one. config/arm9/symbols.txt names it
       _ZN7Vector3D1Ev.

       This makes Vector3 non-POD everywhere, which is nearly free but not
       quite: 10,812 of 10,813 enrolled functions are byte-identical either way,
       and Rabbit::Behavior grows 8 bytes (0x5cc -> 0x5d4). That one is the
       price of the six destructors this unblocks. */
    ~Vector3() {}

    /* Static-init globals spell this `Vector3(x, y, z)`; a real member ctor is
       what makes mwcc emit the retail shape (elementwise stores, then the
       ~Vector3() registration above). Declaring any ctor also flips copy
       lowering from block moves to memberwise in a few TUs -- those sites use
       a local POD twin instead. */
    Vector3() {}
    Vector3(fx32 x_, fx32 y_, fx32 z_) : x(x_), y(y_), z(z_) {}
#endif
} Vector3;

#ifdef __cplusplus
/* 0xc, and the ROM agrees twice over: daKing_Donketu_c's arrays stride by 0xc and
   its __cxa_vec_cleanup calls pass 0xc as the element size. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Vector3_size_must_be_0xc[sizeof(Vector3) == 0xc ? 1 : -1];
#endif
#endif

typedef struct Vector3s {
    s16 x, y, z;
#ifdef __cplusplus
    /* Declared and empty, for the same reason Vector3's is: daHanachan_c destroys an
       array of a 6-byte type through
       __cxa_vec_cleanup(this + 0x444, 5, 6, func_02011508), a POD array needs no
       cleanup, and 0x02011508 is four bytes of `bx lr`. 6 is sizeof(Vector3s). */
    ~Vector3s() {}
    /* Empty default construction: daHanachan_c's factory passes
       func_0203d73c, four bytes of `bx lr`, to __cxa_vec_ctor(this + 0x444,
       5, 6, ...). The body is defined in Vector3s.h, so a C++ file that
       default-constructs a Vector3s (a local, an array, or a class holding
       one) must include that header, or it calls an undefined
       _ZN8Vector3sC1Ev. */
    inline Vector3s();
#endif
} Vector3s;

#ifdef __cplusplus
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Vector3s_size_must_be_0x6[sizeof(Vector3s) == 6 ? 1 : -1];
#endif
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
