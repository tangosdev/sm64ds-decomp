#ifndef DAOBJMARUTA_C_H
#define DAOBJMARUTA_C_H

#include "types.h"
#include "dBgActor_c.h"

/* The abstract base of the rolling logs: the two that roll down a slope in Tall,
 * Tall Mountain and Lethal Lava Land. `maruta` is a log.
 *
 * A LAYER THE TREE DID NOT HAVE. The ROM's RTTI names this class and points it at
 * dBgActor_c; the tree named the base's base for both leaves.
 *
 *   _ZTI13daObjMaruta_c  ov022 0x02114350
 *   _ZTS13daObjMaruta_c  ov022 0x0211435c   "13daObjMaruta_c"
 *   vtable               ov080 0x02128338, 32 slots, same count as the base
 *   kind                 __si_class_type_info, ONE base, subobject offset 0
 *   base                 dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * THE VTABLE LIVES IN ov080, WITH THE CODE, and the typeinfo record does not --
 * the same split daObjFallBlock_c has. Every one of this class's own slots
 * resolves to an ov080 function. The relocations that reach the vtable carry the
 * ambiguous tag `overlays(79,80)`; the config settles it from the other side by
 * marking ov079's copy of 0x02128338 `kind:bss ambiguous`, which cannot hold a
 * vtable, and leaving ov080's as the definition.
 *
 * ABSTRACT IN THREE SLOTS. 0 (InitResources), 3 (CleanupResources) and 6
 * (Behavior) are all null, so each leaf brings its own rolling logic; this class
 * supplies only 9 (Render), 16 (D1), 17 (D0) and 27 (OnHitByMegaChar). The
 * leaves forward those three null slots into the shared ov080 helpers
 * func_ov080_021274ac / 0212714c / 021270dc.
 *
 * TWO DESCENDANTS: daObjFlMaruta_c (daObjFlMaruta_c, ov022) and daObjHmMaruta_c
 * (daObjHmMaruta_c, ov030). Each one's destructor stores this class's vtable between
 * its own and _ZTV10dBgActor_c. Both factories pass 836 = 0x344.
 *
 * THE 0x24 SPAN ABOVE dBgActor_c IS THIS CLASS'S. The shared ov080 helpers
 * (InitResources copies mPos into mRestPos, Behavior rolls mRollAngVel and
 * snaps against mRestPos, BeforeClsn writes mHitPos when the other actor is
 * PLAYER) read the same offsets for the same purpose on both leaves, so the
 * span lives here rather than as pad_320 on each descendant.
 */

#ifdef __cplusplus

struct daObjMaruta_c : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mRestPosX;          /* 0x320 -- InitResources copies mPosX */
    s32 mRestPosY;          /* 0x324 */
    s32 mRestPosZ;          /* 0x328 */
    s32 mHitPosX;           /* 0x32c -- BeforeClsn copies PLAYER mPos */
    s32 mHitPosY;           /* 0x330 */
    s32 mHitPosZ;           /* 0x334 */
    s32 mPathAngDiff;       /* 0x338 -- AngleDiff of travel vs mAngleY */
    s16 mRollAngVel;        /* 0x33c */
    u8  mHitTimer;          /* 0x33e -- DecIfAbove0_Byte */
    u8  pad_33f;
    u16 mSoundTimer;        /* 0x340 -- DecIfAbove0_Short */
    u8  mHitByPlayer;       /* 0x342 */
    u8  pad_343;

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN13daObjMaruta_cD1Ev (which does exist out of line, at ov080 0x02127014).
       An out-of-line declaration here would make each descendant emit a `bl`
       the ROM does not have. */
    virtual ~daObjMaruta_c() {}

    /* Slot 27, this class's own override, defined out of line in
       src/game/actors/d_a_obj_maruta.cpp. LAYOUT-NEUTRAL: it
       re-uses the slot dActor_c already holds rather than appending one.

       Because the destructor above is inline this class has no key function,
       so declaring the first out-of-line virtual makes THIS the key function
       and its translation unit emits _ZTV13daObjMaruta_c, the RTTI records and
       the implicit destructor bodies. The cartridge orders D0 below D1, which
       no admissible source form reproduces, so those two bodies stay in their
       enrolled shards and this TU's copies are licensed deadstrip-duplicate. */
    void OnHitByMegaChar(Player &player);  /* void, see include/Stump.h */

    /* Slot 9, this class's own override. Not `virtual` -- fBase_c already
       declares it virtual, and re-adding the keyword here is a style
       choice this header hasn't made for its own other declarations. */
    s32 Render();
    /* THE NULL SLOTS THE NOTE ABOVE ALREADY NAMES, SPELT SO THE COMPILER AGREES.
       mwccarm lays down a bare 0x00000000 with no relocation for a pure virtual --
       there is no __cxa_pure_virtual in this image for it to point at -- so a zero
       word in a ROM vtable IS the `= 0`, and it is the only thing that produces one.
       Left undeclared, this class silently inherits dBgActor_c's concrete bodies and
       the vtable it emits disagrees with the cartridge at exactly these slots.
       Measured by tools/romdata_check.py, which is the only gate that reads them:
       the ROM build's 106/106 compares .text alone and is blind here.

       DECLARED LAST, AND WITH `virtual` -- unlike the plain overrides above. The
       pure-specifier is only valid on a declaration carrying the keyword, and a
       pure virtual has no body to emit, so it can never become the key function:
       whichever virtual was first and non-inline before is still first now. */
    virtual s32 InitResources() = 0;        /* slot  0 */
    virtual s32 CleanupResources() = 0;     /* slot  3 */
    virtual s32 Behavior() = 0;             /* slot  6 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjMaruta_c_size_must_be_0x344[sizeof(daObjMaruta_c) == 0x344 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJMARUTA_C_H */
