#ifndef DADOSSYCAP_C_H
#define DADOSSYCAP_C_H

#include "CapIcon.h"
#include "dExtSimpleModel_c.h"
#include "dCcAc_c.h"
#include "dActor_c.h"

struct daDossy_c;

/* daDossyCap_c is the ROM's own RTTI name for this class (this tree once
 * coined it DorrieCap): the typeinfo at ov065
 * 0x0211cd4c names dActor_c as the sole base at offset 0, and the class's
 * vtable at 0x0211cdc4 (31 slots, same count as dActor_c's) is what pairs it
 * to daDossyCap_c_classInit, which stores that address after allocating 0x184 bytes
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
 * base, dCapIcon_c, dExtSimpleModel_c, dCcAc_c; destruction is the exact reverse.
 *
 * The header this replaces was deliberately flat -- a non-deriving struct
 * whose leading "fields" duplicated dActor_c's storage -- because giving a
 * non-derived struct a virtual would have inserted a vptr and shifted every
 * offset. Deriving from dActor_c is what makes the declarations below honest.
 *
 * SM64DS RTTI names the implementation daDossyCap_c. The reconstructed
 * factory daDossyCap_c_classInit (historical alias
 * DorrieCap_Spawn) constructs it for the DOSSY_CAP
 * registry profile.
 */
struct daDossyCap_c : dActor_c {
    dCapIcon_c         mCapIcon;                /* 0x0d4 */
    dExtSimpleModel_c mModel;                   /* 0x0f0 */
    dCcAc_c mdCcAc_c;     /* 0x140 */
    /* The Dorrie wearing this cap. Behavior bails out when it is null, and
       otherwise copies the cap position out of her mCapPos triple and its two
       angles out of mHeadRotX and mAngle.y. */
    daDossy_c *mCarrier; /* 0x174 */
    u8                 pad_178[0xc];

    /* Inline is load-bearing: src/game/actors/d_a_dossy.cpp defines the key
       function InitResources, so it emits D1 then D0 with no homeless D2. */
    virtual ~daDossyCap_c() {}

    virtual s32 InitResources();       /* slot 0 */
    virtual s32 Behavior();            /* slot 6 */
    virtual s32 Render();              /* slot 9 */
    virtual int OnYoshiTryEat();       /* slot 18 */

    /* Behavior's per-frame hit test. When mdCcAc_c registers hitFlags
       0x400000 (the player bit) from a grounded player who is not already in
       daPly_c no-control state 5, it spawns the loose cap (actor 0x10d,
       ACTOR_OBJ_MARIO_CAP) ahead of them, relinks mCapIcon with iconKind 0 so
       the real bit clears and Render stops drawing, reloads the carrier's
       mRideCooldown with 30, and returns 1. Returns 0 when nothing was
       released. */
    int ReleaseCapIfTouched();
    void UpdateModelMatrix();
    /* mCapIcon.mFlags bit 1 -- "this node's cap is the real one", documented
       in include/CapIcon.h, where Dorrie's cap is the observed case: Render
       draws only while it is set and tests that bit alone. daDossy_c's
       UpdateHeadDown asks through mCap before letting a rider mount. */
    int IsRealCap();
};

typedef char daDossyCap_c_size_must_be_0x184[
    sizeof(daDossyCap_c) == 0x184 ? 1 : -1];

#endif /* DADOSSYCAP_C_H */
