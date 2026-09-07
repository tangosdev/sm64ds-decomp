#ifndef SEESAWBOB_H
#define SEESAWBOB_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS proves this class as daObjSeesaw_c through RTTI, allocation size
 * and vtable identity. The factory and profile spellings below are
 * reconstructed source-style names -- evidence-bounded proposals, not
 * recovered SM64DS symbols.
 *
 * daObjSeesaw_c_classInit_BOMB_SEESAW at 0x02135f94 (historical alias
 * SeesawBob_Spawn) allocates 0x328 and installs this class's cartridge
 * vtable. It backs the BOMB_SEESAW registry profile, whose descriptor at
 * 0x02137484 is reconstructed as g_profile_BOMB_SEESAW.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct SeesawBob : dBgActor_c {
    u8  mVariant;                     /* 0x31e -- 0..6 from actorID; indexes all three ov095 file tables */
    u8  pad_31f[0x1];
    s32 mTiltSound;                   /* 0x320 -- recycled Sound::PlayLong handle, replayed while |mAngleXSpeed| > 0xa */
    s16 mAngleXSpeed;                 /* 0x324 -- the tilt speed func_ov095_021358cc integrates into mAngleX */
    u8  mPoundedThisFrame;            /* 0x326 -- set by OnGroundPounded, cleared by the last statement of Behavior */

    /* --- vtable --- */
    virtual ~SeesawBob();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    virtual void OnGroundPounded(dActor_c &other); /* slot 21 */
};

typedef char SeesawBob_size_must_be_0x328[sizeof(SeesawBob) == 0x328 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct SeesawBob {
    u8  pad_000[0xc];
    u16 actorID;            /* 0x00c */
    u8  pad_00e[0x7e];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x20];
    u32 mFlags;             /* 0x0b0 */
    u8  pad_0b4[0x20];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~SeesawBob calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x32];
    u8  mVariant;           /* 0x31e */
    u8  pad_31f[0x1];
    s32 mTiltSound;         /* 0x320 */
    s16 mAngleXSpeed;       /* 0x324 */
    u8  mPoundedThisFrame;  /* 0x326 */
};

#endif /* __cplusplus */

#endif /* SEESAWBOB_H */
