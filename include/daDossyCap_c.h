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
 * member's type and offset. The 0xd4 member is a CapIcon: its ctor/dtor are
 * func_ov001_020ab3c4 / func_ov001_020ab3a0, the same pair dCapEnemy_c holds
 * at its own 0x164, typed in include/CapIcon.h. The flat header this replaces
 * read into it as two loose bytes: unk_0d4 (its vtable word) and unk_0ef
 * (mCapIcon.mFlags, 0xd4 + 0x1b).
 *
 * Own vtable slots, from the ROM table diffed against dActor_c's: 0
 * InitResources, 6 Behavior, 9 Render, 16/17 the destructor pair, 18
 * OnYoshiTryEat. No CleanupResources or OnPendingDestroy override, unlike its
 * daKrpa_c/daKpFr_c/daEyBm_c siblings.
 *
 * THE DESTRUCTORS STAY UNMIGRATED, and the reason is order: D1 destroys
 * dCcAc_c, then Model, then the CapIcon -- exact reverse
 * declaration order -- but CapIcon's destructor is still spelt
 * func_ov001_020ab3a0 rather than CapIcon::~CapIcon, so a real ~daDossyCap_c()
 * would have to call it in the body, which runs BEFORE the implicit member
 * destructors instead of after them. dCapEnemy_c got away with exactly that
 * only because its CapIcon is the LAST member (see _ZN11dCapEnemy_cD2Ev.cpp);
 * here it is the first. Until CapIcon's destructor is a real method, the
 * declaration below is satisfied by the extern "C" free functions in
 * _ZN12daDossyCap_cD1Ev.c / D0Ev.c, which also keeps this class's key function
 * undefined in every TU so no object emits a coined-name vtable.
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
    CapIcon            mCapIcon;                /* 0x0d4 */
    Model              mModel;                  /* 0x0f0 */
    dCcAc_c mdCcAc_c;     /* 0x140 */
    /* The actor this cap is riding, a pointer spelt s32. Behavior bails out
       when it is null, and otherwise copies the cap position out of that actor
       0x0d8 triple and its two angles out of the actor +0xe4 and
       dActor_c::mAngleY. [_ZN12daDossyCap_c8BehaviorEv.cpp] */
    s32                mCarrier;                 /* 0x174 */
    u8                 pad_178[0xc];

    /* Declared first on purpose, same reasoning as dActor_c.h: the key
       function pins where mwcc anchors the vtable -- and this one is never
       defined as a method, see the header comment. */
    virtual ~daDossyCap_c();

    virtual s32 InitResources();       /* slot 0 */
    virtual s32 Behavior();            /* slot 6 */
    virtual s32 Render();              /* slot 9 */
    virtual int OnYoshiTryEat();       /* slot 18 */
};

typedef char daDossyCap_c_size_must_be_0x184[
    sizeof(daDossyCap_c) == 0x184 ? 1 : -1];

#endif /* DADOSSYCAP_C_H */
