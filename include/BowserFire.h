/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member types by hand.
 *
 * class BowserFire: 4 matched functions.
 *
 * The three sub-objects are the real classes, and each one's size is pinned by
 * that class's own static assertion, so the offsets below are checked twice --
 * once by the assert, once by closing exactly on the next named field:
 *
 *     dBgCh_Actr        0x110 + 0x1bc = 0x2cc   -> mUniqueID_2cc
 *     dCcAc_c  0x2d0 + 0x034 = 0x304   -> mShadowModel
 *     ShadowModel         0x304 + 0x028 = 0x32c   -> padding
 *
 * There is NO unk_2e8 here any more, and its absence is the point. An earlier
 * revision named 0x2e8 as a BowserFire field on the strength of a single
 * `|= 1`. It is not one: 0x2d0 + 0x18 lands inside mdCcAc_c, and
 * dCc_c::flags is at 0x18 and is documented as "bit 0 makes Update
 * bail" -- which is exactly what setting bit 0 does. It was the sub-object's
 * own field all along, the same mistake as Player's `mBodyClsnFlags`.
 *
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES for the unk_ entries are placeholders. */
#ifndef BOWSERFIRE_H
#define BOWSERFIRE_H
#include "types.h"

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "dBgCh_Actr.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"

struct BowserFire : dEnemyBase_c {
    dBgCh_Actr mWithMeshClsn;                 /* 0x110 */
    s32 mUniqueID_2cc;                                /* 0x2cc */
    dCcAc_c mdCcAc_c;     /* 0x2d0 */
    ShadowModel mShadowModel;                   /* 0x304 */
    u8  pad_32c[0x30];
    s32 mVariant;            /* 0x35c */
    s32 mShadowRadiusScale; /* 0x360 */
    s32 mGroundY;            /* 0x364 */
    u8  pad_368[0x4];
    s32 mTimer;            /* 0x36c */
    u8  pad_370[0x4];
    u16 mFrameCount;            /* 0x374 */
    u8  pad_376[0x2];
    u8  mVariant_378;            /* 0x378 */
    u8  mDropsShadow;       /* 0x379 */
    u8  pad_37a[0x2];
    s32 mParticleHandle_37c;            /* 0x37c */
    s32 mParticleHandle_380;            /* 0x380 */
    s32 mSoundHandle;            /* 0x384 */
    s32 mSoundID;            /* 0x388 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~BowserFire();              /* slots 0 (D1), 1 (D0) */

    /* --- non-virtual --- */
    int InitResources();
    int CleanupResources();
    int Render();
    int Behavior();
};

typedef char BowserFire_size_must_be_0x38c[sizeof(BowserFire) == 0x38c ? 1 : -1];

#else

struct BowserFire {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x34];
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x64];
    u8  mWithMeshClsn;            /* 0x110 */
    u8  pad_111[0x1bb];
    s32 mUniqueID_2cc;            /* 0x2cc */
    u8  mdCcAc_c;            /* 0x2d0 */
    u8  pad_2d1[0x33];
    u8  mShadowModel;            /* 0x304 */
    u8  pad_305[0x57];
    s32 mVariant;            /* 0x35c */
    s32 mShadowRadiusScale; /* 0x360 */
    s32 mGroundY;            /* 0x364 */
    u8  pad_368[0x4];
    s32 mTimer;            /* 0x36c */
    u8  pad_370[0x4];
    u16 mFrameCount;            /* 0x374 */
    u8  pad_376[0x2];
    u8  mVariant_378;            /* 0x378 */
    u8  mDropsShadow;       /* 0x379 */
    u8  pad_37a[0x2];
    s32 mParticleHandle_37c;            /* 0x37c */
    s32 mParticleHandle_380;            /* 0x380 */
    s32 mSoundHandle;            /* 0x384 */
    s32 mSoundID;            /* 0x388 */
};

#endif /* __cplusplus */

#endif
