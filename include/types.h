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

/* 20.12 fixed-point scalar, as used by the SDK/game maths.

   NOT named `Fix12`. In the original C++ that is a class TEMPLATE, not a scalar typedef;
   this tree's convention spells it out in mangled names, e.g.
   `_ZN4cstd5atan2E5Fix12IiES1_` for `cstd::atan2(Fix12<int>, Fix12<int>)`. That is a
   convention and not evidence: the image holds zero `_Z...` strings, so no function's
   mangled name here is ROM-derived (its RTTI carries class names only). Where a
   by-value `5Fix12IiE` parameter contradicts the bytes, the bytes win -- see
   notes/mwccarm-codegen.md 6az. Taking the name here for a plain `s32` would
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
       `__cxa_vec_cleanup(this + 0x3e8, 8, 0xc, func_020072c0)` can only exist if
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
   its __cxa_vec_cleanup calls pass 0xc as the element size. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Vector3_size_must_be_0xc[sizeof(Vector3) == 0xc ? 1 : -1];
#endif
#endif

typedef struct Vector3s {
    s16 x, y, z;
#ifdef __cplusplus
    /* Declared and empty, for the same reason Vector3's is: Wiggler destroys an
       array of a 6-byte type through
       __cxa_vec_cleanup(this + 0x444, 5, 6, func_02011508), a POD array needs no
       cleanup, and 0x02011508 is four bytes of `bx lr`. 6 is sizeof(Vector3s). */
    ~Vector3s() {}
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

/* THE BASE'S TAIL PADDING, CLAIMED BY THE DERIVED CLASS.

   mwccarm follows the Itanium C++ ABI here: a derived class's first member may
   be laid down inside the base's tail padding, at the base's dsize (its last
   field's end) rather than at sizeof(base). dBgActor_c's fields stop at 0x31e
   and its sizeof rounds to 0x320, so mwccarm starts a derived class at 0x31e
   and every header that wants its first derived field at the ROM's 0x320 has
   to spell those two bytes itself.

   MSVC does not reuse tail padding. It starts the derived class at
   sizeof(base) == 0x320, so the SAME two bytes of pad push every derived field
   two bytes up, and the first 4-aligned one lands at 0x324 rather than 0x320.
   Nothing in the build can see that: the class still compiles, the byte gate
   only ever compiles the mwccarm arm, and every member access is simply four
   bytes off in the host binary. Run link100's boot sweep is where it surfaced,
   as nine levels faulting on a null vtable pointer inside ModelBase::SetFile --
   the model member each actor's factory had constructed at the ROM offset while
   the header read it four bytes further on.

   So the pad is spelled through this macro: emitted for the compiler that needs
   it, absent for the one that does not, and a single name to grep for. Both
   arms put the following member at the ROM offset, which is the point; use
   ROM_OFFSET_ASSERT below to pin that per class.

       struct Derived : dBgActor_c {
           ROM_BASE_TAIL_PAD(31e, 0x2)
           ShadowModel mShadowModel;   / * 0x320 * /
       }; */
#ifdef _MSC_VER
#define ROM_BASE_TAIL_PAD(at, size)
#else
#define ROM_BASE_TAIL_PAD(at, size) u8 pad_##at[size];
#endif

/* Pin a member to the offset the cartridge reads it at. C++ only, host only:
   the matching build compiles some of these headers as C, where offsetof on a
   class with base classes is not available, and the ROM arm is already pinned
   by the whole-object size asserts each header carries. */
#if defined(__cplusplus) && defined(_MSC_VER)
#define ROM_OFFSET_ASSERT(type, member, off)     static_assert(offsetof(type, member) == (off),                   #type "::" #member " is not at the ROM offset " #off)
#else
#define ROM_OFFSET_ASSERT(type, member, off)
#endif

#endif /* TYPES_H */
