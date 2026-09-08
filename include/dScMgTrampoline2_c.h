/* class dScMgTrampoline2_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgTrampoline2_c : dScMgD3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable is ov006:0x0213fc7c. The last of
 * dScMgD3DBase_c's four children, and the largest class in the family.
 *
 * SIZE 0x7bac, from dScMgTrampoline2_c_classInit's own
 * `_ZN7fBase_cnwEj(0x7bac)`. MgTrampolineTerror_Spawn is the historical
 * English-minigame alias.
 *
 * SIX ARRAYS, and factory and destructor agree on all six in exact reverse
 * order: built 0x500c, 0x5458, 0x5ddc, 0x6ffc, 0x7164, 0x7ad0; destroyed
 * 0x7ad0, 0x7164, 0x6ffc, 0x5ddc, 0x5458, 0x500c. Two element destructors
 * are shared with dScMgTrampoline_c (func_ov006_020d1008 for the
 * 0x32c-stride array, func_ov006_02120938 for the 5 * 0x24 one) and one
 * with dScMgJump2_c (func_ov006_020eed64) -- which is what a shared base's
 * children are supposed to look like.
 *
 * Two spans remain byte storage pending typed-field reconstruction (#2497).
 * Behavior uses 0x7ac4 as a Particle::System unique ID and 0x7ac8 as a
 * fixed-point ramp; 0x7acc is still unexplained. That unknown word does not
 * rule out typing the first two. All six words at 0x7b84..0x7b9c have matched
 * accesses in this TU's initialization and state functions. Their original
 * member names are unknown; the storage is live, not unused padding.
 *
 * The current arrays retain raw storage and six reverse-order cleanup calls.
 * Typed components and their lifecycle remain reconstruction work (#2497).
 * The inline destructor body preserves the observed D1/D0 emission.
 * dScMgD3DBase_c supplies operator delete. */
#ifndef DSCMGTRAMPOLINE2_C_H
#define DSCMGTRAMPOLINE2_C_H
#include "dScMgD3DBase_c.h"

extern "C" void __cxa_vec_cleanup(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020ca604(void);
extern "C" void func_ov006_020d1008(void);
extern "C" void func_ov006_020eed64(void);
struct Model;
extern "C" int func_ov006_021227c8(char *object);
extern "C" Model *func_ov006_02122c68(Model *model);
extern "C" void func_ov006_02120938(void);

struct dScMgTrampoline2_c : dScMgD3DBase_c {
    virtual ~dScMgTrampoline2_c() {
        __cxa_vec_cleanup(mArray6, 5, 0x24, (void *)func_ov006_02120938);
        __cxa_vec_cleanup(mArray5, 0x14, 0x78, (void *)func_ov006_02122c68);
        __cxa_vec_cleanup(mArray4, 0xa, 0x24, (void *)func_ov006_020eed64);
        __cxa_vec_cleanup(mArray3, 0xa, 0x1d0, (void *)func_ov006_021227c8);
        __cxa_vec_cleanup(mArray2, 3, 0x32c, (void *)func_ov006_020d1008);
        __cxa_vec_cleanup(mArray1, 5, 0xdc, (void *)func_ov006_020ca604);
    }
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    virtual int  OnAttacked2();                        /* slot 23 */
    virtual int  OnKicked();                           /* slot 24 */
    virtual int  OnPushed();                           /* slot 25 */
    virtual void Virtual88(int cx, int cy, int colour, int size); /* slot 34 */

    /* 0x5004 -- eight-byte state PMF, invoked by Behavior with this class as
       receiver. The five table targets already have bodies in the TU:
       02123b20, 02124088, 02123b24, 02123cb4 and 02123bf4 (all ov006).
       Converting their raw records and this field together to a typed state
       interface remains issue #2497; their original method names are unknown. */
    u8  pad_5004[0x8];   /* 0x5004 -- the state callback; see the block above */
    u8  mArray1[0x44c];   /* 0x500c -- 5 * 0xdc,    elem dtor func_ov006_020ca604 */
    u8  mArray2[0x984];   /* 0x5458 -- 3 * 0x32c,   elem dtor func_ov006_020d1008 */
    u8  mArray3[0x1220];  /* 0x5ddc -- 0xa * 0x1d0, elem dtor func_ov006_021227c8 */
    u8  mArray4[0x168];   /* 0x6ffc -- 0xa * 0x24,  elem dtor func_ov006_020eed64 */
    u8  mArray5[0x960];   /* 0x7164 -- 0x14 * 0x78, elem dtor func_ov006_02122c68 */
    u8  pad_7ac4[0xc];    /* 0x7ac4 -- Behavior reads 0x7ac4 and 0x7ac8; see banner */
    u8  mArray6[0xb4];    /* 0x7ad0 -- 5 * 0x24,    elem dtor func_ov006_02120938 */
    u8  pad_7b84[0x18];   /* 0x7b84 -- six live words; see banner */
    s16 unk_7b9c;         /* 0x7b9c */
    s16 unk_7b9e;         /* 0x7b9e */
    s16 unk_7ba0;         /* 0x7ba0 */
    s16 unk_7ba2;         /* 0x7ba2 */
    u8  pad_7ba4[0x4];    /* 0x7ba4 */
    s16 unk_7ba8;         /* 0x7ba8 */
    u8  unk_7baa;         /* 0x7baa */
    u8  unk_7bab;         /* 0x7bab */

    /* These definitions override existing base slots without adding fields.
       The production TU defines the non-inline virtuals and emits the vtable;
       its manifest owns and verifies that table. The shared base supplies
       reconstructed callback names and signatures, not original-name proof. */
    s32 InitResources();      /* slot 0 -- src/minigames/d_s_mg_trampoline2.cpp */
    s32 CleanupResources();   /* slot 3 -- ov006 0x0212318c */
    s32 Behavior();           /* slot 6 -- ov006 0x02123340 */
    s32 Render();             /* slot 9 -- ov006 0x021231ac */
};

typedef char dScMgTrampoline2_c_size_must_be_0x7bac[sizeof(dScMgTrampoline2_c) == 0x7bac ? 1 : -1];

#endif
