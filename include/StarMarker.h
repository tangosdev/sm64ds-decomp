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
#include "types.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"

struct StarMarker {
    u8  pad_000[0x4];
    s32 mUniqueID;            /* 0x004 -- fBase_c's own uniqueID.
                                   SpawnRedCoinStarIfNecessary hands it to the
                                   star it spawns, at star+0x434. */
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x3c];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
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
    /* 0x18c..0x1bb is ONE Matrix4x3, written whole: Behavior stores
       IDENTITY_MATRIX4X3 over `*(Mtx *)&mShadowMtx` and then fills in the
       translation row, and hands &mShadowMtx to dActor_c::DropShadowRadHeight
       as the shadow's matrix. It stays four separate members rather than a
       typed one because that is the spelling the bytes reproduce. */
    u8  mShadowMtx;              /* 0x18c -- first byte of that matrix */
    u8  pad_18d[0x23];
    s32 mShadowMtxTX;            /* 0x1b0 -- its translation row, set to
                                     mPos >> 3 every frame */
    s32 mShadowMtxTY;            /* 0x1b4 */
    s32 mShadowMtxTZ;            /* 0x1b8 */
    s32 mSpawnPosX;              /* 0x1bc -- mPos as InitResources found it.
                                     Written there and read nowhere in the
                                     tree; the name records the copy. */
    s32 mSpawnPosY;              /* 0x1c0 */
    s32 mSpawnPosZ;              /* 0x1c4 */
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
    s32 mHitActor;               /* 0x1d0 -- the actor that touched this
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
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnPendingDestroy();
    void SpawnRedCoinStarIfNecessary();
#endif
};

typedef char StarMarker_size_must_be_0x1dc[sizeof(struct StarMarker) == 0x1dc ? 1 : -1];

#endif
