#ifndef DADOSSYCAP_C_H
#define DADOSSYCAP_C_H

#include "CapIcon.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "dActor_c.h"

/* daDossyCap_c is the ROM's own RTTI name for this class (this tree once
 * coined it DorrieCap): the typeinfo at ov065
 * 0x0211cd4c names dActor_c as the sole base at offset 0, and the class's
 * vtable at 0x0211cdc4 (31 slots, same count as dActor_c's) is what pairs it
 * to DorrieCap_Spawn, which stores that address after allocating 0x184 bytes
 * via fBase_c::operator new.
 *
 * The Spawn constructs the three owned subobjects below at 0xd4..0x140 in
 * declaration order; D1 destroys them in exactly the reverse order before
 * chaining to dActor_c::~dActor_c -- two independent witnesses for each
 * member's type and offset. The 0xd4 member is a dCapIcon_c: its constructor
 * and destructor are the same pair dCapEnemy_c holds
 * at its own 0x164, typed in include/CapIcon.h. The flat header this replaces
 * read into it as two loose bytes: unk_0d4 (its vtable word) and unk_0ef
 * (mCapIcon.mFlags, 0xd4 + 0x1b).
 *
 * Own vtable slots, from the ROM table diffed against dActor_c's: 0
 * InitResources, 6 Behavior, 9 Render, 16/17 the destructor pair, 18
 * OnYoshiTryEat. No CleanupResources or OnPendingDestroy override, unlike its
 * daKrpa_c/daKpFr_c/daEyBm_c siblings.
 *
 * The ROM's dCapIcon_c RTTI and vtable now identify its constructor and
 * destructor by their original class name. That declaration is what finally
 * lets the compiler reproduce this class's lifecycle: construction proceeds
 * base, dCapIcon_c, Model, dCcAc_c; destruction is the exact reverse.
 *
 * The header this replaces was deliberately flat -- a non-deriving struct
 * whose leading "fields" duplicated dActor_c's storage -- because giving a
 * non-derived struct a virtual would have inserted a vptr and shifted every
 * offset. Deriving from dActor_c is what makes the declarations below honest.
 */
struct daDossyCap_c : dActor_c {
    /* Used both as a Player* (Behavior chases it through +0x360) and as a raw
       word copied into the spawned cap actor's own 0xd0. Kept s32 as the flat
       header had it; the pointer reads go through explicit casts. */
    s32                mEatingPlayer;           /* 0x0d0 */
    dCapIcon_c         mCapIcon;                /* 0x0d4 */
    Model              mModel;                  /* 0x0f0 */
    dCcAc_c mdCcAc_c;     /* 0x140 */
    /* The actor this cap is riding, a pointer spelt s32. Behavior bails out
       when it is null, and otherwise copies the cap position out of that actor
       0x0d8 triple and its two angles out of the actor +0xe4 and
       dActor_c::mAngleY. [_ZN12daDossyCap_c8BehaviorEv.cpp] */
    s32                mCarrier;                 /* 0x174 */
    u8                 pad_178[0xc];

    /* Inline is load-bearing: explicit use in the destructor source files
       emits D1 then D0 without inventing a homeless D2. */
    virtual ~daDossyCap_c() {}

    virtual s32 InitResources();       /* slot 0 */
    virtual s32 Behavior();            /* slot 6 */
    virtual s32 Render();              /* slot 9 */
    virtual int OnYoshiTryEat();       /* slot 18 */
};

typedef char daDossyCap_c_size_must_be_0x184[
    sizeof(daDossyCap_c) == 0x184 ? 1 : -1];

#endif /* DADOSSYCAP_C_H */
