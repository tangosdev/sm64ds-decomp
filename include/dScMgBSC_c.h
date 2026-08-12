/* class dScMgBSC_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgBSC_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). English Spawn-function name is MgLuckyStars_Spawn;
 * kept as the symbol name for the factory (already attributed), but the
 * class itself takes its real ROM identity, matching this tree's current
 * convention.
 *
 * SIZE 0x51d0, from MgLuckyStars_Spawn.cpp's own
 * `_ZN9ActorBasenwEj(0x51d0)`.
 *
 * ONE SHARED MEMBER at 0x4f38, size 0x270 (func_ov006_020c1d80/020c1c64),
 * shared verbatim by five other classes in this family -- left as raw
 * bytes, see include/dScMgMemory_c.h's own note.
 *
 * ONE TRIVIAL ARRAY at 0x51a8, 2 elements of 8 bytes each -- the
 * destructor calls `__destroy_arr(c+0x51a8, 2, 8, NullDestructor_0203d47c)`
 * and the factory constructs it with `func_020733a8(p+0x51a8, 2, 8,
 * func_0203d738, NullDestructor_0203d47c)`. A no-op destructor means POD
 * elements -- left as raw bytes, element type not evidenced.
 *
 * OWN TAIL, 0x51b8..0x51d0: one field is real matched access
 * (src/func_ov006_0212527c.cpp, .../02125364.c), the rest stays pad.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN10dScMgBSC_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGBSC_C_H
#define DSCMGBSC_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void NullDestructor_0203d47c(void);

struct dScMgBSC_c : dScMgSingle3DBase_c {
    virtual ~dScMgBSC_c();

    u8  mTable[0x270];   /* 0x4f38 -- ctor func_ov006_020c1d80, dtor func_ov006_020c1c64 */
    u8  mArray[0x10];     /* 0x51a8 -- 2 * 8 bytes, ctor func_0203d738, dtor NullDestructor_0203d47c */
    s32 unk_51b8;         /* 0x51b8 */
    u8  pad_51bc[0x14];   /* 0x51bc */
};

typedef char dScMgBSC_c_size_must_be_0x51d0[sizeof(dScMgBSC_c) == 0x51d0 ? 1 : -1];

#endif
