/* Started life AUTO-GENERATED from matched-function evidence by
 * tools/gen_header.py; the field names below have since been recovered from
 * the bodies. Offsets/widths are observed, not guessed. Gaps are explicit
 * padding. Renaming cannot change codegen.
 *
 * The on-screen glint that shows where an uncollected star will appear.
 * mParam's low nibble is the star id and its next nibble picks the flavour --
 * InitResources turns that into mState (0 spins a plain star model, 1 is the
 * markable one, 2 and 3 are the two collision-driven variants) and into the
 * mFlags bits everything else tests.
 *
 * 0x004, 0x05c..0x064, 0x08e and 0x0cc ARE fBase_c's and dActor_c's OWN
 * LAYOUT, not this class's, and are named from include/dActor_c.h by offset.
 *
 * Provenance table: notes/butterfly-tornado-provenance.md. */
#ifndef STARMARKER_H
#define STARMARKER_H
#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "math/Matrix.h"

/* RTTI calls this class daStarBase_c. StarMarker is the readable compatibility
 * spelling already fixed by the matched function names. The ROM's
 * __si_class_type_info record gives it one dActor_c base at offset zero, and
 * its 31-slot vtable has the same extent as dActor_c's. Only resource,
 * behavior, render, pending-destroy, and destructor slots are overridden. */
struct StarMarker : dActor_c {
    u8 pad_0d0[0x4];
    /* dCcAcPos_c member. The cartridge's own ~StarMarker calls _ZN10dCcAcPos_cD1Ev at
       +0x0d4 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dCcAcPos_c mdCcAcPos_c;            /* 0x0d4 */
    /* Model member, named by _ZN5ModelD1Ev at +0x114 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_154 (+0x40 = mat4x3.t.x),
       unk_158 (+0x44 = mat4x3.t.y), unk_15c (+0x48 = mat4x3.t.z), which the header
       declared separately inside it. */
    Model mModel;            /* 0x114 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x164 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10StarMarkerD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x164 */
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
    s32 mSpawnedActorID;         /* 0x1cc -- a unique id, not a pointer:
                                     OnPendingDestroy feeds it to
                                     dActor_c::FindWithID and, if that actor
                                     has no death-table slot of its own,
                                     clears mSpawnedDeathTableID's bit.
                                     InitResources zeroes it; nothing in the
                                     tree ever sets it non-zero, so the write
                                     side is still missing. */
    dActor_c *mHitActor;         /* 0x1d0 -- the actor that touched this
                                     marker, resolved from
                                     mdCcAcPos_c.otherOwner by Behavior just
                                     before it calls func_ov002_020e7d84.
                                     A dActor_c*, stored through an int. */
    u16 mAppearTimer;            /* 0x1d4 */
    s16 mSpawnedDeathTableID;    /* 0x1d6 -- the death-table slot
                                     OnPendingDestroy clears for
                                     mSpawnedActorID. InitResources sets -1,
                                     the "no slot" value dActor_c uses for its
                                     own mDeathTableID. */
    u8  mState;            /* 0x1d8 */
    u8  mStarID;            /* 0x1d9 */
    u8  pad_1da[0x1];
    u8  mFlags;            /* 0x1db */
    virtual ~StarMarker();
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();

    void SpawnRedCoinStarIfNecessary();
    /* Readable inferred name, not a ROM-authenticated original spelling.
     * Address/ownership evidence is recorded in symbols/actor_renames.tsv. */
    void Collect();
};

typedef char StarMarker_size_must_be_0x1dc[sizeof(struct StarMarker) == 0x1dc ? 1 : -1];

#endif
