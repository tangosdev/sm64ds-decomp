/* class dScMgTrampoline2_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgTrampoline2_c : dScMgD3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable is ov006:0x0213fc7c. The last of
 * dScMgD3DBase_c's four children, and the largest class in the family.
 *
 * SIZE 0x7bac, from MgTrampolineTerror_Spawn's own
 * `_ZN7fBase_cnwEj(0x7bac)`. The English _Spawn name is already
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
 * TWO GAPS between arrays stay padding, but only ONE is still unevidenced.
 * 0x7b84..0x7b9c (0x18, after the last array) is: no matched access reaches
 * it, checked for split-literal forms as well as whole constants. THE OTHER
 * IS NOT, AND THE ORIGINAL CLAIM WAS AN ARTEFACT OF THE SEARCH. 0x7ac4..0x7ad0
 * (0xc, between mArray5 and mArray6) is read by this class's own Behavior --
 * 0x7ac4 as a Particle::System unique ID, 0x7ac8 as a fix12 ramped towards
 * 0x14000 -- which the sweep missed because that function was still called
 * func_ov006_02123340 and nothing tied it to this class. It stays one pad
 * rather than becoming two fields plus a tail: the third word is still
 * unreached, and cutting the gap two-thirds of the way along would assert a
 * boundary nothing shows.
 *
 * No Model and no typed member, so nothing here is compiler-generated --
 * the destructor body is all six calls and nothing else.
 *
 * The current split production sources still provide the destructor out of
 * line. A reconstructed whole TU defines DSCMGTRAMPOLINE2_INLINE_DTOR before
 * including this header to test the final inline form: that is the spelling
 * which emits the retail D1/D0 order without inventing a homeless D2. No
 * separate operator delete is needed: dScMgD3DBase_c provides one. */
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
#ifdef DSCMGTRAMPOLINE2_INLINE_DTOR
    virtual ~dScMgTrampoline2_c() {
        __destroy_arr(mArray6, 5, 0x24, (void *)func_ov006_02120938);
        __destroy_arr(mArray5, 0x14, 0x78, (void *)func_ov006_02122c68);
        __destroy_arr(mArray4, 0xa, 0x24, (void *)func_ov006_020eed64);
        __destroy_arr(mArray3, 0xa, 0x1d0, (void *)func_ov006_021227c8);
        __destroy_arr(mArray2, 3, 0x32c, (void *)func_ov006_020d1008);
        __destroy_arr(mArray1, 5, 0xdc, (void *)func_ov006_020ca604);
    }
#else
    virtual ~dScMgTrampoline2_c();
#endif
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    virtual int  OnAttacked2();                        /* slot 23 */
    virtual int  OnKicked();                           /* slot 24 */
    virtual int  OnPushed();                           /* slot 25 */
    virtual void Virtual88(int cx, int cy, int colour, int size); /* slot 34 */

    /* 0x5004 -- the state callback, a pointer-to-member of this class, which
       is mwccarm's eight bytes exactly; Behavior calls through it every frame.
       Same field at the same offset as dScMgJump2_c's, which is what two
       children of dScMgD3DBase_c sharing a base ought to look like. Was "no
       matched access", true only of the files NAMED after this class -- the
       access was in func_ov006_02123340 all along. Left as bytes rather than
       declared as the member pointer: naming it would fix a signature for
       every state function in the table, and none of them is recovered. */
    u8  pad_5004[0x8];   /* 0x5004 -- the state callback; see the block above */
    u8  mArray1[0x44c];   /* 0x500c -- 5 * 0xdc,    elem dtor func_ov006_020ca604 */
    u8  mArray2[0x984];   /* 0x5458 -- 3 * 0x32c,   elem dtor func_ov006_020d1008 */
    u8  mArray3[0x1220];  /* 0x5ddc -- 0xa * 0x1d0, elem dtor func_ov006_021227c8 */
    u8  mArray4[0x168];   /* 0x6ffc -- 0xa * 0x24,  elem dtor func_ov006_020eed64 */
    u8  mArray5[0x960];   /* 0x7164 -- 0x14 * 0x78, elem dtor func_ov006_02122c68 */
    u8  pad_7ac4[0xc];    /* 0x7ac4 -- Behavior reads 0x7ac4 and 0x7ac8; see banner */
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

    /* --- this class's own vtable overrides, defined out of line under their
       own mangled names. Each re-uses a slot fBase_c already holds rather
       than appending one, and none adds a field, so the size assert below is
       untouched. The destructor above stays the key function, so no
       translation unit starts emitting _ZTV18dScMgTrampoline2_c because of
       these. Signatures are include/fBase_c.h's and include/dScMgBase_c.h's
       own, copied unchanged.

       Behavior IS THE MATCHED ACCESS THAT REACHES pad_7ac4 and pad_5004; the
       banner above records what that costs the two "no matched access"
       claims. --- */
    s32 InitResources();      /* slot 0 -- src/_ZN18dScMgTrampoline2_c13InitResourcesEv.cpp */
    s32 CleanupResources();   /* slot 3 -- ov006 0x0212318c */
    s32 Behavior();           /* slot 6 -- ov006 0x02123340 */
    s32 Render();             /* slot 9 -- ov006 0x021231ac */
};

typedef char dScMgTrampoline2_c_size_must_be_0x7bac[sizeof(dScMgTrampoline2_c) == 0x7bac ? 1 : -1];

#endif
