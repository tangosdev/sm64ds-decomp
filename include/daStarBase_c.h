/* Started life AUTO-GENERATED from matched-function evidence by
 * tools/gen_header.py; the field names below have since been recovered from
 * the bodies. Offsets/widths are observed, not guessed. Gaps are explicit
 * padding. Renaming cannot change codegen.
 *
 * The on-screen glint that shows where an uncollected star will appear.
 * param1's low nibble is the star id and its next nibble picks the flavour --
 * InitResources turns that into mState (0 spins its own model; 1, 2 and 3 use
 * the other model and sit still, with state 1 from the odd kinds, 2 from
 * kind 4 and 3 from kind 5, which also differ in the touch volume they set
 * up) and into the mFlags bits everything else tests.
 *
 * 0x004, 0x05c..0x064, 0x08e and 0x0cc ARE fBase_c's and dActor_c's OWN
 * LAYOUT, not this class's, and are named from include/dActor_c.h by offset.
 *
 * The ROM TU boundary is shared with daStar_c and StarCamera; this class
 * header does not claim a standalone original source file. */
#ifndef DASTARBASE_C_H
#define DASTARBASE_C_H
#include "dActor_c.h"
#include "dExtSimpleModel_c.h"
#include "dExtShadowModel_c.h"
#include "dCcAcPos_c.h"
#include "math/Matrix.h"

/* RTTI calls this class daStarBase_c. daStarBase_c is the readable compatibility
 * spelling already fixed by the matched function names. The ROM's
 * __si_class_type_info record gives it one dActor_c base at offset zero, and
 * its 31-slot vtable has the same extent as dActor_c's. Only resource,
 * behavior, render, pending-destroy, and destructor slots are overridden. */
struct daStarBase_c : dActor_c {
    u8 pad_0d0[0x4];
    /* dCcAcPos_c member. The cartridge's own ~daStarBase_c calls _ZN10dCcAcPos_cD1Ev at
       +0x0d4 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dCcAcPos_c mdCcAcPos_c;            /* 0x0d4 */
    /* dExtSimpleModel_c member, named by _ZN17dExtSimpleModel_cD1Ev at +0x114 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_154 (+0x40 = mat4x3.t.x),
       unk_158 (+0x44 = mat4x3.t.y), unk_15c (+0x48 = mat4x3.t.z), which the header
       declared separately inside it. */
    dExtSimpleModel_c mModel; /* 0x114 */
    /* dExtShadowModel_c member, named by the class's own destructor calling
       dExtShadowModel_c's D1 at +0x164 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12daStarBase_cD0Ev, now in src/game/actors/d_a_star.cpp] */
    dExtShadowModel_c mShadowModel;            /* 0x164 */
    Matrix4x3 mShadowMtx;        /* 0x18c -- shadow transform */
    Vector3 mSpawnPos;           /* 0x1bc -- mPos as InitResources found it.
                                     Written there and read nowhere in the
                                     tree; the name records the copy. */
    s32 mGroundY;                /* 0x1c8 -- the ground height under the
                                     marker: InitResources raycasts down with
                                     a dBgCh_Gnd from mPosY + 0x1e000 and
                                     stores the result's own +0x44. Behavior
                                     turns mPosY - mGroundY into the shadow's
                                     drop height. */
    u32 mLinkedStarID;           /* 0x1cc -- uniqueID of the star (daStar_c) this
                                     marker is linked to, 0 for none. Stored by
                                     LinkSilverStarAndStarMarker; OnPendingDestroy
                                     feeds it to dActor_c::FindWithID and, if that
                                     star has no death-table slot of its own,
                                     clears mLinkedStarDeathTableID's bit. */
    dActor_c *mHitActor;         /* 0x1d0 -- the actor that touched this
                                     marker, resolved from
                                     mdCcAcPos_c.otherOwner by Behavior just
                                     before it calls Collect().
                                     A dActor_c*, stored through an int. */
    u16 mAppearTimer;            /* 0x1d4 -- appear delay, in frames: Behavior sets
                                     it to 0x2a while this marker's star id is not
                                     the next one in data_0209f344[data_0209f208]
                                     (VS_STAR_SPAWN_ORDER indexed by
                                     NUM_VS_STARS_COLLECTED), counts it down once
                                     it is, and shows the marker at 0 */
    s16 mLinkedStarDeathTableID; /* 0x1d6 -- the linked star's mDeathTableID, kept
                                     when it is not negative; -1 when unlinked,
                                     the "no slot" value dActor_c uses for its
                                     own mDeathTableID. */
    u8  mState;            /* 0x1d8 */
    u8  mStarID;            /* 0x1d9 */
    u8  mLinkedStarState;   /* 0x1da -- the linked star's daStar_c::mState when
                                LinkSilverStarAndStarMarker ran */
    /* mFlags bits, all evidenced by Init/Behavior/Collect and the daStar_c
       code that reaches into them. */
    struct FlagBits {
        u8 hold : 1;        /* 0x01 -- while set Behavior's appear logic does not
                                show the marker (Init sets it when kind & 3 == 3) */
        u8 visible : 1;     /* 0x02 -- Render, the drop shadow and the touch volume
                                need it; Collect clears it */
        u8 spawned : 1;     /* 0x04 -- SpawnRedCoinStarIfNecessary spawns only while
                                it is clear, then sets it */
        u8 refresh : 1;     /* 0x08 -- "decide the appear timer again": Behavior
                                handles it and clears it */
        u8 pad_4 : 4;
    };
    union {
        u8  mFlags;            /* 0x1db */
        FlagBits mBits;
    };
    virtual ~daStarBase_c() {}
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();

    void SpawnRedCoinStarIfNecessary();
    /* Readable inferred name, not a ROM-authenticated original spelling.
     * Address/ownership evidence is recorded in symbols/actor_renames.tsv. */
    void Collect();

    /* Receivers. The address is the method name. */
    void LinkSilverStarAndStarMarker(char* b);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char StarMarker_size_must_be_0x1dc[sizeof(struct daStarBase_c) == 0x1dc ? 1 : -1];
#endif

#endif
