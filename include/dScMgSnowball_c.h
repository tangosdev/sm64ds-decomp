/* class dScMgSnowball_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgSnowball_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x0214000c; the RTTI
 * pointer one word below it is _ZTI15dScMgSnowball_c, which is what ties
 * that table to this class rather than to a neighbour.
 *
 * SIZE 0xc59c, and the allocation is NOT in this class's own constructor.
 * func_ov006_021295ac takes an already-allocated `char*` -- it is a C2,
 * not a factory. Its one caller, MgSnowballSlalom_Spawn, is where
 * `_ZN7fBase_cnwEj(0xc59c)` lives. Deriving a size from the field span
 * would have stopped at 0xbe94 and been 0x708 short.
 *
 * CONSTRUCTOR AND DESTRUCTOR AGREE MEMBER FOR MEMBER, in exact reverse
 * order: mModel at 0xaba4, then mArray1 at 0xacd8 (0x80 elements of 8),
 * mArray2 at 0xb5d8 (0x80 of 8), mArray3 at 0xba14 (0x20 of 0x24). The
 * destructor destroys mArray3, mArray2, mArray1, then mModel. Nothing is
 * left over either way.
 *
 * mModel IS A REAL, TYPED Model, and two independent witnesses fix it:
 * the constructor calls `_ZN5ModelC1Ev(t + 0xaba4)` and the destructor
 * calls `_ZN5ModelD1Ev(c + 0xaba4)`, and `sizeof(Model)` is 0x50, which
 * closes exactly on 0xabf4 -- the very next word the constructor writes
 * (`*(int*)(t + 0xabf4) = 0`). That is the "two witnesses that close on
 * each other with nothing left over" bar. The Model destruction is
 * therefore left COMPILER-GENERATED here rather than written out: mModel
 * is declared before the three arrays, so reverse-declaration order puts
 * its destructor call last, which is exactly where the ROM has it.
 *
 * THE THREE ARRAYS stay raw bytes -- no source in the tree reads inside
 * an element, so no element type is evidenced. mArray1 and mArray2 share
 * an element ctor/dtor pair (func_0203d738 / NullDestructor_0203d47c), so
 * they are the same element type as each other; mArray3's is its own
 * (func_ov006_0212968c / func_ov006_02125800).
 *
 * THE 0xb0d8..0xb5d8 REGION IS FOUR PARALLEL ARRAYS, not padding, and they
 * tile the 0x500 exactly: Snowball's own Render (src/func_ov006_02127d10.c)
 * indexes 0xb0d8 as `*(int*)(c + i*4 + ...)`, 0xb2d8 and 0xb358 as
 * `*(u8*)(c + i + ...)`, and 0xb3d8 as `*(int*)(c + i*4 + ...)`; 0x80
 * elements each gives 0x200 + 0x80 + 0x80 + 0x200 = 0x500, landing on
 * mArray2. src/func_ov006_02125bbc.c writes the first two the same way.
 *
 * 0xbe94..0xc59c IS A FIFTH ARRAY, and it is why this class has NO trailing
 * slack at all: src/func_ov006_02129690.c already carries the full element
 * layout (two s32 then three u8 at +0x1e/0x1f/0x20, stride 0x24) as a local
 * `Elem arr[50]` at exactly this offset, and 50 * 0x24 = 0x708 closes
 * precisely on the allocation literal. At least eight further ov006 files
 * index it. Unlike the other four arrays neither structor touches it, so
 * its element type is trivially constructible -- which is exactly why the
 * structors alone could never have found it.
 *
 * So the field span and the `operator new` literal now AGREE for this
 * class. The size does not rest on the literal alone.
 *
 * mMenuOpen, which the old auto-generated header declared, is dScMgBase_c's
 * own and is already declared there; it is dropped from this file, not
 * lost. src/func_ov006_02128fb8.c reads it through this class and keeps
 * working because it inherits it.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN15dScMgSnowball_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGSNOWBALL_C_H
#define DSCMGSNOWBALL_C_H
#include "dScMgSingle3DBase_c.h"
#include "Model.h"

extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_02125800(void);
extern "C" void NullDestructor_0203d47c(void);

struct dScMgSnowball_c : dScMgSingle3DBase_c {
    virtual ~dScMgSnowball_c();

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources();     /* slot 0 -- ov006 0x02129268 */
    virtual s32 CleanupResources();  /* slot 3 -- ov006 0x021291f8 */
    virtual s32 Behavior();          /* slot 6 -- ov006 0x021283a4 */
    virtual s32 Render();            /* slot 9 -- ov006 0x02127d10 */

    u8    pad_4f38[0x5c04]; /* 0x4f38 -- no matched access */
    s32   unk_ab3c;         /* 0xab3c */
    u8    pad_ab40[0x8];    /* 0xab40 */
    s32   unk_ab48;         /* 0xab48 */
    s32   unk_ab4c;         /* 0xab4c */
    u8    pad_ab50[0x18];   /* 0xab50 */
    s32   unk_ab68;         /* 0xab68 */
    s32   unk_ab6c;         /* 0xab6c */
    u8    pad_ab70[0x30];   /* 0xab70 */
    s32   unk_aba0;         /* 0xaba0 */
    Model mModel;           /* 0xaba4 -- 0x50, see file banner */
    s32   unk_abf4;         /* 0xabf4 -- the constructor's own `= 0` write */
    u8    pad_abf8[0xe0];   /* 0xabf8 */
    u8    mArray1[0x400];   /* 0xacd8 -- 0x80 * 8, elem dtor NullDestructor_0203d47c */
    s32   unk_b0d8[0x80];   /* 0xb0d8 -- parallel array, see file banner */
    u8    unk_b2d8[0x80];   /* 0xb2d8 */
    u8    unk_b358[0x80];   /* 0xb358 */
    s32   unk_b3d8[0x80];   /* 0xb3d8 */
    u8    mArray2[0x400];   /* 0xb5d8 -- 0x80 * 8, elem dtor NullDestructor_0203d47c */
    s32   unk_b9d8;         /* 0xb9d8 */
    s32   unk_b9dc;         /* 0xb9dc */
    u8    pad_b9e0[0x14];   /* 0xb9e0 */
    s32   unk_b9f4;         /* 0xb9f4 */
    u8    unk_b9f8;         /* 0xb9f8 */
    u8    pad_b9f9[0x3];    /* 0xb9f9 */
    s32   unk_b9fc;         /* 0xb9fc */
    s32   unk_ba00;         /* 0xba00 */
    s32   unk_ba04;         /* 0xba04 */
    u8    pad_ba08[0x4];    /* 0xba08 */
    s32   unk_ba0c;         /* 0xba0c */
    s32   unk_ba10;         /* 0xba10 */
    u8    mArray3[0x480];   /* 0xba14 -- 0x20 * 0x24, elem dtor func_ov006_02125800 */
    u8    mArray4[0x708];   /* 0xbe94 -- 50 * 0x24, trivial elements, see file banner */
};

typedef char dScMgSnowball_c_size_must_be_0xc59c[sizeof(dScMgSnowball_c) == 0xc59c ? 1 : -1];

#endif
