#ifndef DAOBJPILE_C_H
#define DAOBJPILE_C_H

#include "dBgActor_c.h"

/* PILE, the stump. It starts with three steps to sink: a ground pound sinks it
 * one step (60 units), a mega hit sinks all that remain, and reaching the
 * bottom drops coins.
 *
 * BASE: dBgActor_c, direct -- _ZTI11daObjPile_c at ov091 0x0213527c names
 * _ZTI10dBgActor_c.
 *
 * TWO WITNESSES, and they close on each other:
 *
 *   daObjPile_c_classInit  fBase_c::operator new(816 = 0x330),
 *                 dBgActor_c::dBgActor_c(), stores _ZTV11daObjPile_c,
 *                 then the members below in this order.
 *   ~daObjPile_c  the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x330 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daObjPile_c : dBgActor_c {
    /* dBgActor_c ends at 0x31e (its own sizeof rounds up to 0x320); these two
       bytes are this class's own, in the base's tail padding -- same reuse
       include/daObjFl_Ring_c.h documents at the same offset. mStepsLeft is
       read off its uses: InitResources sets 3, each ordinary ground pound decrements it,
       a mega hit sinks the pile by mStepsLeft steps and zeroes it, and both
       hit handlers ignore the pile at 0. The other names are placeholders:
       offsets, widths and read/write sites are observed from
       Behavior/OnGroundPounded/OnHitByMegaChar (slots 6/21/27, ov091
       0x02133738/3648/35d4), not proven semantics. */
    u8 mStepsLeft; /* 0x31e -- 3 when intact, 0 once fully sunk */
    u8 mAttackCooldown; /* 0x31f -- set to 15 by a hit, counted down in Behavior; hits ignored while nonzero */
    u8 mBusy; /* 0x320 -- suppresses OnHitByMegaChar and the coin drops; daWanwan_c sets it to 1 on the stump it spawns */
    u8 pad_321[3];
    s32 mAngleDelta; /* 0x324 -- accumulated turn-toward-player angle */
    s16 mPrevAngle;  /* 0x328 -- last frame's horizontal angle to player */
    u8 pad_32a[2];
    /* KINOKO_TAG backlink: daObjKinokoTag_c::Behavior stores itself here, and
       daObjPile_c::NotifyTagPileGone reads it back. */
    dActor_c *mLinkedTag; /* 0x32c */

    virtual ~daObjPile_c(); /* slots 16 (D1), 17 (D0) */

    virtual s32 CleanupResources(); /* slot  3 */
    virtual s32 Render();           /* slot  9 */
    virtual s32 InitResources();    /* slot  0 */
    virtual s32 Behavior();         /* slot  6 */
    /* OnGroundPounded and OnHitByMegaChar return `void`, not `int`: both
       overrides are what proved include/dActor_c.h's slots 21 and 27 wrong
       (each compares class fields and returns early, and mwcc allocates
       registers differently for `int` vs `void` even with r0 untouched) --
       measured with tools/mangle.py, not assumed. */
    void OnGroundPounded(dActor_c &other); /* slot 21 */
    void OnHitByMegaChar(Player &player);  /* slot 27 */

    /* Non-virtual; coined names. NotifyTagPileGone (ov091 0x02133498) tells
       the linked KINOKO_TAG its pile is gone. Sink (0x021334b8) drops the
       pile one step, or every step when flag is set. flag is int: the callee
       compares the full register. */
    void NotifyTagPileGone();
    void Sink(int flag);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjPile_c_size_must_be_0x330[sizeof(daObjPile_c) == 0x330 ? 1 : -1];
#endif

#endif
