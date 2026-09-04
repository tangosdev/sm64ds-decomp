/* class dScMgBSC_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgBSC_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). The coined English Spawn-function name was
 * MgLuckyStars_Spawn; the factory now carries the reconstructed
 * dScMgBSC_c_classInit spelling, and the class itself takes its
 * real ROM identity, matching this tree's current convention.
 *
 * SIZE 0x51d0, from MgLuckyStars_Spawn.cpp's own
 * `_ZN7fBase_cnwEj(0x51d0)`.
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
 * (func_ov006_0212527c and func_ov006_02125364), the rest stays pad.
 *
 * THE DESTRUCTOR IS DEFINED INLINE, in the class body, and the ROM's own
 * layout is the evidence for it. This class is a leaf with no RTTI
 * descendants, so nothing forces the choice from below -- but _ZN10dScMgBSC_cD1Ev
 * sits at 0x02124908 and _ZN10dScMgBSC_cD0Ev at 0x0212497c, D1 first. An
 * out-of-line destructor does not produce that order: mwcc emits the
 * synthesized D0 ahead of the written D1, which is backwards from the
 * cartridge. Inline in the class body, the destructor cannot be the key
 * function, so InitResources (the next virtual declared, and non-inline)
 * becomes it; that TU emits _ZTV10dScMgBSC_c, whose slots 16 and 17 odr-use
 * D1 then D0, and the pair comes out in exactly the cartridge's order. Same
 * mechanism as dScMgSingle3DBase_c one level up -- see that header's note.
 *
 * The two calls in the body are member destruction, not hand-written
 * cleanup: mArray is declared after mTable and is destroyed first, which is
 * reverse-declaration order. They are spelled out only because both members
 * are still raw byte arrays with no element type recovered.
 *
 * No separate operator delete is needed -- dScMgBase_c, two levels up,
 * already provides one.
 *
 * SM64DS RTTI names the implementation dScMgBSC_c. The reconstructed factory
 * dScMgBSC_c_classInit (historical alias MgLuckyStars_Spawn) installs this class's
 * cartridge vtable for the MG_BS_CARD registry profile.
 */
#ifndef DSCMGBSC_C_H
#define DSCMGBSC_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void NullDestructor_0203d47c(void);

struct dScMgBSC_c : dScMgSingle3DBase_c {
    virtual ~dScMgBSC_c() {
        __destroy_arr(mArray, 2, 8, (void *)NullDestructor_0203d47c);
        func_ov006_020c1c64((char *)mTable);
    }

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources(); /* slot 0 -- ov006 0x0212551c */
    virtual s32 Behavior();   /* slot 6 -- ov006 0x021254c0 */
    virtual s32 Render();     /* slot 9 -- ov006 0x021253bc */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    virtual void OnGroundPounded();                    /* slot 21 */

    u8  mTable[0x270];   /* 0x4f38 -- ctor func_ov006_020c1d80, dtor func_ov006_020c1c64 */
    u8  mArray[0x10];     /* 0x51a8 -- 2 * 8 bytes, ctor func_0203d738, dtor NullDestructor_0203d47c */
    s32 unk_51b8;         /* 0x51b8 */
    u8  pad_51bc[0x14];   /* 0x51bc */
};

typedef char dScMgBSC_c_size_must_be_0x51d0[sizeof(dScMgBSC_c) == 0x51d0 ? 1 : -1];

#endif
