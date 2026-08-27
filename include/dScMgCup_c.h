/* class dScMgCup_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgCup_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x0213c154; the RTTI
 * pointer one word below it is _ZTI10dScMgCup_c, which is what ties that
 * table to this class rather than to a neighbour.
 *
 * THIS CLASS HAD NO HEADER AT ALL before this file -- unlike its twelve
 * siblings it never got an auto-generated skeleton, so there are no
 * inherited field names to carry over and no `.c` includers to convert.
 * Every offset below comes from the two structors, nothing from a prior
 * header.
 *
 * SIZE 0x5470, from the factory's own `_ZN7fBase_cnwEj(0x5470)`
 * (src/unnamed/ov006/020e/func_ov006_020e0574.cpp).
 *
 * THE FACTORY AND THE DESTRUCTOR AGREE MEMBER FOR MEMBER, which is the
 * bar this tree sets (see include/dScMgCard_c.h's own note). The factory
 * constructs, in order: the 0x4f38 table (func_ov006_020c33dc), then
 * mArray1 at 0x50e8 (0x20 elements of 0x18, element ctor
 * func_ov006_020e0634), then mArray2 at 0x53e8 (3 elements of 8, element
 * ctor func_0203d738). The destructor destroys exactly those three in
 * reverse order, with the matching element destructors
 * (NullDestructor_0203d47c, func_ov006_020deac4, func_ov006_020c3288).
 * Nothing is left over either way.
 *
 * TABLE at 0x4f38 is opaque, but it does NOT run all the way to mArray1 as
 * the first draft of this header claimed. Three s32 sit at 0x50d4, spelled
 * as the SPLIT LITERAL `*(int*)(c + 0x5000 + 0xd4 / 0xd8 / 0xdc)` in
 * func_ov006_020df3bc.c and 020dfeec.c -- a form no single-literal grep
 * finds, which is exactly how the wrong bound got written. So the table is
 * bounded at 0x19c, and 8 bytes stay unclaimed between those three fields
 * and mArray1.
 *
 * OWN TAIL, 0x5400..0x5470, IS THE CLASS'S STATE MACHINE, not the
 * unclaimed trailing space the first draft of this header called it. The
 * first statement of Cup's own Behavior (src/minigames/dScMgCup_c/_ZN10dScMgCup_c8BehaviorEv.cpp,
 * vtable slot 6) is a pointer-to-member dispatch through
 * `data_ov006_02141870[*(int*)(o + 0x5418)]`; InitResources
 * (func_ov006_020e0308.cpp, slot 0) already carries a local struct naming
 * `ones[3]` at 0x540c, `ids[3]` at 0x5420 and `flags[3]` at 0x5465; and
 * slot 9's Render reads 0x5462 and 0x5468. Matched access runs to 0x5469.
 *
 * The size is unaffected -- 0x5470 is the factory's literal either way --
 * but the span and the literal AGREE here, which the first draft denied.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/minigames/dScMgCup_c/_ZN10dScMgCup_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGCUP_C_H
#define DSCMGCUP_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c3288(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020deac4(void);
extern "C" void NullDestructor_0203d47c(void);

struct dScMgCup_c : dScMgSingle3DBase_c {
    virtual ~dScMgCup_c();

    /* This class's own overrides, read off the ROM's vtable at ov006 0x0213c154:
       the two slots where the table differs from dScMgSingle3DBase_c's. Spelled
       WITHOUT the `virtual` keyword, the way include/daObjMarioCap_c.h and
       include/daObjRc_Dorifu_c.h spell theirs -- an override of a virtual an
       ancestor already declares is implicitly virtual either way, so each reuses
       an existing slot and adds no field, and the 0x5470 assert below still
       holds. The destructor above is declared first and out of line, so it stays
       this class's KEY FUNCTION and neither of these translation units emits
       _ZTV10dScMgCup_c. */
    s32 InitResources();  /* slot  0 -- src/minigames/dScMgCup_c/_ZN10dScMgCup_c13InitResourcesEv.cpp */
    s32 Behavior();       /* slot  6 -- src/minigames/dScMgCup_c/_ZN10dScMgCup_c8BehaviorEv.cpp */
    s32 Render();         /* slot  9 -- src/minigames/dScMgCup_c/_ZN10dScMgCup_c6RenderEv.cpp */

    u8  pad_4f38[0x19c];  /* 0x4f38 -- opaque table, see file banner */
    s32 unk_50d4;         /* 0x50d4 -- split-literal access, see file banner */
    s32 unk_50d8;         /* 0x50d8 */
    s32 unk_50dc;         /* 0x50dc */
    u8  pad_50e0[0x8];    /* 0x50e0 -- no matched access */
    u8  mArray1[0x300];   /* 0x50e8 -- 0x20 * 0x18, elem dtor func_ov006_020deac4 */
    u8  mArray2[0x18];    /* 0x53e8 -- 3 * 8, elem dtor NullDestructor_0203d47c.
                             func_ov006_020e0308.cpp already recovers the element
                             as a pair of s32; left raw here, own change. */
    u8  pad_5400[0xc];    /* 0x5400 -- no matched access */
    s32 mOnes[3];         /* 0x540c -- named `ones[3]` by InitResources' own struct */
    s32 mState;           /* 0x5418 -- Behavior's pointer-to-member dispatch index */
    u8  pad_541c[0x4];    /* 0x541c */
    s32 mIds[3];          /* 0x5420 -- named `ids[3]` by InitResources' own struct */
    u8  pad_542c[0x8];    /* 0x542c */
    s32 unk_5434[3];      /* 0x5434 -- three parallel per-cup arrays indexed */
    s32 unk_5440[3];      /* 0x5440    together by Behavior's own loop */
    s32 unk_544c[3];      /* 0x544c */
    u8  pad_5458[0xa];    /* 0x5458 */
    u8  unk_5462[3];      /* 0x5462 */
    u8  mFlags[3];        /* 0x5465 -- named `flags[3]` by InitResources' own struct */
    u8  unk_5468;         /* 0x5468 */
    u8  pad_5469[0x7];    /* 0x5469 -- rounds up to the 0x5470 boundary */
};

typedef char dScMgCup_c_size_must_be_0x5470[sizeof(dScMgCup_c) == 0x5470 ? 1 : -1];

#endif
