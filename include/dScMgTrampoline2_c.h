/* class dScMgTrampoline2_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgTrampoline2_c : dScMgD3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable is ov006:0x0213fc7c. The last of
 * dScMgD3DBase_c's four children, and the largest class in the family.
 *
 * SIZE 0x7bac, from MgTrampolineTerror_Spawn's own
 * `_ZN9ActorBasenwEj(0x7bac)`. The English _Spawn name is already
 * attributed and is kept.
 *
 * SIX ARRAYS, and factory and destructor agree on all six in exact reverse
 * order: built 0x500c, 0x5458, 0x5ddc, 0x6ffc, 0x7164, 0x7ad0; destroyed
 * 0x7ad0, 0x7164, 0x6ffc, 0x5ddc, 0x5458, 0x500c. Two element destructors
 * are shared with dScMgTrampoline_c (func_ov006_020d1008 for the
 * 0x32c-stride array, func_ov006_02120938 for the 5 * 0x24 one) and one
 * with dScMgJump2_c (func_ov006_020eed64) -- which is what a shared base's
 * children are supposed to look like.
 *
 * TWO GAPS between arrays are unevidenced and stay padding: 0x7ac4..0x7ad0
 * (0xc, between mArray5 and mArray6) and 0x7b84..0x7b9c (0x18, after the
 * last array). No matched access in this class's own methods reaches
 * either, checked for split-literal forms as well as whole constants.
 *
 * No Model and no typed member, so nothing here is compiler-generated --
 * the destructor body is all six calls and nothing else.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf. No separate operator
 * delete is needed: dScMgD3DBase_c, the immediate base, provides one. */
#ifndef DSCMGTRAMPOLINE2_C_H
#define DSCMGTRAMPOLINE2_C_H
#include "dScMgD3DBase_c.h"

extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020ca604(void);
extern "C" void func_ov006_020d1008(void);
extern "C" void func_ov006_020eed64(void);
extern "C" void func_ov006_021227c8(void);
extern "C" void func_ov006_02122c68(void);
extern "C" void func_ov006_02120938(void);

struct dScMgTrampoline2_c : dScMgD3DBase_c {
    virtual ~dScMgTrampoline2_c();

    u8  pad_5004[0x8];    /* 0x5004 -- no matched access in this class's methods */
    u8  mArray1[0x44c];   /* 0x500c -- 5 * 0xdc,    elem dtor func_ov006_020ca604 */
    u8  mArray2[0x984];   /* 0x5458 -- 3 * 0x32c,   elem dtor func_ov006_020d1008 */
    u8  mArray3[0x1220];  /* 0x5ddc -- 0xa * 0x1d0, elem dtor func_ov006_021227c8 */
    u8  mArray4[0x168];   /* 0x6ffc -- 0xa * 0x24,  elem dtor func_ov006_020eed64 */
    u8  mArray5[0x960];   /* 0x7164 -- 0x14 * 0x78, elem dtor func_ov006_02122c68 */
    u8  pad_7ac4[0xc];    /* 0x7ac4 -- no matched access, see banner */
    u8  mArray6[0xb4];    /* 0x7ad0 -- 5 * 0x24,    elem dtor func_ov006_02120938 */
    u8  pad_7b84[0x18];   /* 0x7b84 -- no matched access, see banner */
    s16 unk_7b9c;         /* 0x7b9c */
    s16 unk_7b9e;         /* 0x7b9e */
    s16 unk_7ba0;         /* 0x7ba0 */
    s16 unk_7ba2;         /* 0x7ba2 */
    u8  pad_7ba4[0x4];    /* 0x7ba4 */
    s16 unk_7ba8;         /* 0x7ba8 */
    u8  unk_7baa;         /* 0x7baa */
    u8  unk_7bab;         /* 0x7bab */
};

typedef char dScMgTrampoline2_c_size_must_be_0x7bac[sizeof(dScMgTrampoline2_c) == 0x7bac ? 1 : -1];

#endif
