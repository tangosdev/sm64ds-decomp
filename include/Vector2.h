#ifndef VECTOR2_H
#define VECTOR2_H

#include "types.h"

/* A 2D Fix12 vector, the 8-byte sibling of types.h's Vector3. The name is
   coined (Vector3's pattern; the SM64DS hacking headers spell it the same).

   THE ARM9 2-VECTOR LIBRARY IS WHERE ITS STRUCTORS LIVE. 0x0203d388..0x0203d738
   is one run of {x, y} Fix12 helpers: rotate by an angle (func_0203d388),
   normalize (func_0203d434 / func_0203d480), cross (func_0203d4d0,
   func_0203d524), dot (func_0203d570), squared length (func_0203d5bc),
   distance (func_0203d5dc), Vec2_Len, scale (func_0203d630, func_0203d680),
   Vec2_Sub and add (func_0203d704). The two empty bodies the ROM passes for
   arrays of an 8-byte element sit inside that run: NullDestructor_0203d47c
   between the normalizers, func_0203d738 right after the add. That is the
   same arrangement as Vector3's empty constructor, func_0203d384, right after
   Vec3_Add.

   Every array of this element the ROM builds or tears down holds Fix12
   positions or velocities (dScMgBSC_c's card positions, read back `>> 12`
   for the meter; the amida, cup, smartball and snowball scenes'
   position/velocity arrays). Each one goes through __cxa_vec_ctor(...,
   func_0203d738, NullDestructor_0203d47c) and __cxa_vec_cleanup(...,
   NullDestructor_0203d47c), and global ones register NullDestructor_0203d47c
   with __register_global_object.

   BOTH STRUCTORS ARE EMPTY AND INLINE, as Vector3's are. Across every module's
   relocations the two addresses are only ever loaded (37 relocations, all
   address loads; Vector3's pair shows the same), never called: the static
   initializers that register a
   global of this type call no constructor first, so the constructor cannot
   be out of line. A declared, empty constructor is still what makes mwccarm
   pass a callback to __cxa_vec_ctor, and a declared destructor is what makes
   an array of it need __cxa_vec_cleanup at all.

   Each TU that builds an array emits its own vague-linkage copies,
   _ZN7Vector2C1Ev and _ZN7Vector2D1Ev. They are the cartridge's arm9 bodies
   at 0x0203d738 and 0x0203d47c, so the TU's manifest discards them as
   deadstrip-duplicate rows against those two addresses (as daDpLift_c's does
   for _ZN7Vector3C1Ev). */
typedef struct Vector2 {
    Fix12i x, y;
#ifdef __cplusplus
    Vector2() {}
    /* Statics initialised with a value spell it `Vector2(x, y)`, as Vector3's
       do: the inline member ctor gives the retail shape, two stores and then
       the ~Vector2() registration (unit020b2c84's banner effect origins). */
    Vector2(Fix12i x_, Fix12i y_) : x(x_), y(y_) {}
    ~Vector2() {}
#endif
} Vector2;

#ifdef __cplusplus
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Vector2_size_must_be_0x8[sizeof(Vector2) == 8 ? 1 : -1];
#endif
#endif

#endif
