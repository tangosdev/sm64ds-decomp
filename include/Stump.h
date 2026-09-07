#ifndef STUMP_H
#define STUMP_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjPile_c_classInit  fBase_c::operator new(816 = 0x330), dBgActor_c::dBgActor_c(), stores _ZTV5Stump,
 *                 then the members below in this order.
 *   ~Stump   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x330 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct Stump : dBgActor_c {
    /* dBgActor_c ends at 0x31e (its own sizeof rounds up to 0x320); these two
       bytes are this class's own, in the base's tail padding -- same reuse
       include/VolcanoRing.h documents at the same offset. Field names are
       placeholders: offsets, widths and read/write sites are observed from
       Behavior/OnGroundPounded/OnHitByMegaChar (slots 6/21/27, ov091
       0x02133738/3648/35d4), not proven semantics. */
    u8  mState;              /* 0x31e -- gates OnGroundPounded/OnHitByMegaChar; ==3 in Behavior */
    u8  mAttackCooldown;      /* 0x31f -- DecIfAbove0_Byte-gated */
    u8  mBusy;                /* 0x320 -- suppresses OnHitByMegaChar and part of Behavior */
    u8  pad_321[0x3];
    s32 mAngleDelta;          /* 0x324 -- accumulated turn-toward-player angle */
    s16 mPrevAngle;           /* 0x328 -- last frame's horizontal angle to player */
    u8  pad_32a[0x6];

    virtual ~Stump();            /* slots 16 (D1), 17 (D0) */

    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Render();                /* slot  9 */

    /* Slots 0/6/21/27. Attributed by the vtable, not by the pre-migration
       `recovered name:` comments. None is the key function: ~Stump() stays
       the first out-of-line virtual, so none of these four TUs newly emits
       _ZTV5Stump.

       OnGroundPounded and OnHitByMegaChar return `void`, not `int`: both
       overrides are what proved include/dActor_c.h's slots 21 and 27 wrong
       (each compares class fields and returns early, and mwcc allocates
       registers differently for `int` vs `void` even with r0 untouched) --
       measured with tools/mangle.py, not assumed. Same falsifiability shape
       as slot 24's prior BigBrickBlock::OnKicked correction. */
    virtual s32 InitResources();               /* slot  0 */
    virtual s32 Behavior();                     /* slot  6 */
    void OnGroundPounded(dActor_c &other);      /* slot 21 */
    void OnHitByMegaChar(Player &player);       /* slot 27 */
};

typedef char Stump_size_must_be_0x330[sizeof(Stump) == 0x330 ? 1 : -1];

#endif /* STUMP_H */
