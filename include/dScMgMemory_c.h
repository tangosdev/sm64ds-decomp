/* class dScMgMemory_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgMemory_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json).
 *
 * THIS CLASS WAS CARRYING A COINED ENGLISH NAME, AND THAT IS WHY IT LOOKED
 * DONE. Its destructor pair shipped as `_ZN13MgMemoryMatchD1Ev`/`D0Ev` and
 * its vtable as `_ZTV13MgMemoryMatch` -- real-looking mangled names for a
 * class the ROM never called that. The ROM settles it: the vptr value is
 * ov006:0x0213d1b8, and the RTTI pointer one word below it is
 * `_ZTI13dScMgMemory_c`, whose `_ZTS` string reads "13dScMgMemory_c". Same
 * class, two names. The class takes its real ROM identity here, matching
 * this tree's convention and its six already-migrated siblings; the factory
 * keeps `MgMemoryMatch_Spawn`, which is already attributed, exactly as
 * include/dScMgCard_c.h does for MgPicturePoker_Spawn.
 *
 * The rename is length-neutral, which is a small mercy: "MgMemoryMatch" and
 * "dScMgMemory_c" are both 13 characters, so every mangled name keeps its
 * `_ZN13`/`_ZTV13` prefix and no length arithmetic changes.
 *
 * SIZE 0x5340, from MgMemoryMatch_Spawn's own `_ZN7fBase_cnwEj(0x5340)`.
 *
 * SHARED TABLE at 0x4f38, size 0x270, constructed by func_ov006_020c1d80
 * and destroyed by func_ov006_020c1c64 -- the same pair, at the same
 * offset, that dScMgCard_c, dScMgMCarlo_c and dScMgMCarlo2_c use. (Card's
 * own banner points here for the shared-table note; until now this file was
 * still the auto-generated skeleton and had no note to point at.) Those
 * three siblings each name two fields WITHIN the table, unk_4f52 and
 * unk_511e, because their own methods read them. NONE of this class's eight
 * recovered methods reads inside the table at all, so nothing is named here
 * and the whole 0x270 stays opaque.
 *
 * 0x51a8..0x5314 HAS NO MATCHED ACCESS IN ANY OF THIS CLASS'S OWN METHODS.
 * That is a statement about the eight files this tree has recovered for
 * dScMgMemory_c -- checked for split-literal forms like `c + 0x5000 + 0x1a4`
 * as well as whole constants -- and NOT a proof that no code in the ROM
 * touches it. Left as padding.
 *
 * OWN TAIL: three fields carried over verbatim from this header's previous
 * auto-generated form, all real matched access out of this class's own
 * methods, at unchanged offsets and widths. The three fields the old header
 * declared below 0x4f38 (0x0a8, 0x0ac, 0x0b4) are dScMgBase_c's own and are
 * already declared there.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN13dScMgMemory_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGMEMORY_C_H
#define DSCMGMEMORY_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int func_ov006_020c1c64(char *t); /* decl_common.h's own signature */

struct dScMgMemory_c : dScMgSingle3DBase_c {
    virtual ~dScMgMemory_c();
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    virtual void OnGroundPounded();                    /* slot 21 */

    u8  pad_4f38[0x270];   /* 0x4f38 -- shared table, opaque here; see banner */
    u8  pad_51a8[0x16c];   /* 0x51a8 -- no matched access, see banner */
    s32 unk_5314;          /* 0x5314 */
    s32 unk_5318;          /* 0x5318 */
    u8  pad_531c[0x1f];    /* 0x531c */
    u8  unk_533b;          /* 0x533b */
    u8  pad_533c[0x4];     /* 0x533c -- rounds up to the 0x5340 boundary */

    /* --- this class's own vtable overrides, defined out of line under their
       own mangled names. Each re-uses a slot fBase_c already holds rather
       than appending one, and none adds a field, so the size assert below is
       untouched. The destructor above stays the key function, so no
       translation unit starts emitting _ZTV13dScMgMemory_c because of these.
       Signatures are include/fBase_c.h's and include/dScMgBase_c.h's own,
       copied unchanged. --- */
    s32 InitResources();      /* slot 0 -- ov006 0x020f53e4 */
    s32 Behavior();           /* slot 6 -- ov006 0x020f5388 */
    s32 Render();             /* slot 9 -- ov006 0x020f5324 */
};

typedef char dScMgMemory_c_size_must_be_0x5340[sizeof(dScMgMemory_c) == 0x5340 ? 1 : -1];

#endif
