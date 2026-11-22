/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member types by hand.
 *
 * class daKpaFire_c: Bowser's fire (KOOPAFIRE, ov060). Every function is
 * defined in src/game/actors/d_a_kpa_fire.cpp; its banner lists the eight variants.
 *
 * The three sub-objects are the real classes, and each one's size is pinned by
 * that class's own static assertion, so the offsets below are checked twice --
 * once by the assert, once by closing exactly on the next named field:
 *
 *     dBgCh_Actr         0x110 + 0x1bc = 0x2cc   -> mKpaUniqueID
 *     dCcAc_c            0x2d0 + 0x034 = 0x304   -> mShadowModel
 *     dExtShadowModel_c  0x304 + 0x028 = 0x32c   -> mShadowMat (shadow matrix)
 *
 * There is NO unk_2e8 here any more, and its absence is the point. An earlier
 * revision named 0x2e8 as a daKpaFire_c field on the strength of a single
 * `|= 1`. It is not one: 0x2d0 + 0x18 lands inside mdCcAc_c, and
 * dCc_c::flags is at 0x18 and is documented as "bit 0 makes Update
 * bail" -- which is exactly what setting bit 0 does. It was the sub-object's
 * own field all along, the same mistake as daPly_c's `mBodyClsnFlags`.
 *
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 *
 * What the daKpaFire_c-specific fields do (read off src/game/actors/d_a_kpa_fire.cpp):
 *   mVariant        param1 & 7: which of eight behaviours this fire runs. The
 *                   per-variant handler/init pairs are the pointer-to-member
 *                   tables data_ov060_0211afb4 / data_ov060_0211af74, filled by
 *                   __sinit_d_a_kpa_fire.cpp at 0x02119df0 (the .cpp banner
 *                   lists them).
 *   mSubVariant     (param1 >> 4) & 3, a sub-selector several variants read: it
 *                   indexes the terminal-velocity and particle-id tables and
 *                   switches speeds and spawn choices.
 *   mFireScale      fx32 size of the fire (0x1000 = 1.0). It is the particle
 *                   scale and, times mShadowRadiusMul, the shadow radius.
 *   mKpaUniqueID    uniqueID of a daKpa_c (actor id 0x117), zero from
 *                   InitResources. daKpa_c's own spawn code writes its uniqueID
 *                   here right after spawning a variant 0; variant 4 instead
 *                   looks the daKpa_c up by actor id on its first frame and
 *                   stores that daKpa_c's uniqueID. Variant 0 looks the actor up by it.
 *   mLanded         zero until a variant 6/7 fire first touches the ground,
 *                   one from then on.
 *   mTickCount      Behavior calls so far, bumped BEFORE the variant handler
 *                   runs (InitResources never writes it, so it starts from the
 *                   zero-filled allocation).
 *   mFrameCount     Behavior calls so far, bumped AFTER the handler runs, so a
 *                   handler sees it one behind mTickCount.
 *   mPhaseOffset    start phase of the sideways wobble: the wobble helper
 *                   (ApplySideWobble) adds it to mTickCount. The variant 1,
 *                   2 and 4 inits set it randomly, but only variant 2's
 *                   Behavior reaches the wobble with it; variants 6/7 wobble
 *                   with it still 0.
 *   mGroundY        floor height found under the spawn position (the shadow is
 *                   drawn there), or the spawn y when no floor was found.
 *   mParticleHandle, mParticleHandle2
 *                   the two particle handles KeepParticlePair keeps alive:
 *                   `kind` in the first, `kind + 1` in the second. Variants 6/7
 *                   also start the falling particle in the first directly.
 *   mSoundHandle, mSoundID
 *                   the looping sound variant 0 plays (Sound::PlayLong); the
 *                   handle is reset when the id changes.
 * The 0x32c..0x35c block is mShadowMat, the Matrix4x3 UpdateShadow fills
 * each frame for the shadow. */
#ifndef DAKPAFIRE_C_H
#define DAKPAFIRE_C_H
#include "types.h"

#include "dEnemyBase_c.h"
#include "dBgCh_Actr.h"
#include "dCcAc_c.h"
#include "dExtShadowModel_c.h"

/* ROM identity (SM64DS proves daKpaFire_c, not the coined BowserFire):
 * _ZTI11daKpaFire_c at ov060 0x0211a7b4, _ZTS11daKpaFire_c at 0x0211a7c0
 * ("11daKpaFire_c"), _ZTV11daKpaFire_c at 0x0211a7f4 whose typeinfo word
 * points at that _ZTI. daKpaFire_c_classInit (0x02117938, KOOPAFIRE profile)
 * stores this vtable; the D1/D0 below fill its slots 16/17. */
struct daKpaFire_c : dEnemyBase_c {
    dBgCh_Actr mWithMeshClsn;       /* 0x110 */
    s32 mKpaUniqueID;               /* 0x2cc */
    dCcAc_c mdCcAc_c;               /* 0x2d0 */
    dExtShadowModel_c mShadowModel; /* 0x304 */
    Matrix4x3 mShadowMat;           /* 0x32c -- shadow Matrix4x3 storage */
    s32 mVariant;                   /* 0x35c */
    s32 mFireScale;                 /* 0x360 -- fx32 */
    s32 mGroundY;                   /* 0x364 */
    s32 mShadowRadiusMul;           /* 0x368 -- integer multiplier; handlers store 0x10 */
    s32 mLanded;                    /* 0x36c */
    s32 mTickCount;                 /* 0x370 */
    u16 mFrameCount;                /* 0x374 */
    u16 mPhaseOffset;               /* 0x376 */
    u8  mSubVariant;                /* 0x378 */
    u8  mDropsShadow;               /* 0x379 -- zero only for variant 0 */
    u8  pad_37a[0x2];
    s32 mParticleHandle;            /* 0x37c */
    s32 mParticleHandle2;           /* 0x380 */
    s32 mSoundHandle;               /* 0x384 */
    s32 mSoundID;                   /* 0x388 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~daKpaFire_c();              /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    int InitResources();
    int CleanupResources();
    int Render();
    int Behavior();

    void UpdateShadow();        /* per-frame drop shadow */
    void UpdateRideMouth();     /* variant 0 handler */
    void EndBurnOut();          /* variant 6/7 handler */
    int FrameCountExceeds(unsigned int n);
    void BurnPlayer();
    void InitFall();
    void InitTossUp();
    void ApplySideWobble();
    void UpdateFallAndBurn();
    void InitSpray();
    void UpdateSpray();
    void InitFlight();
    void UpdateFlight();
    void InitGrow();
    void UpdateGrowAndSplit();
    void InitBounce();
    void UpdateBounce();
    void UpdateScatter();
    void InitNoCylinder();
    void KeepParticlePair(u32 kind, int resize, int yOffset);
};

typedef char daKpaFire_c_size_must_be_0x38c[sizeof(daKpaFire_c) == 0x38c ? 1 : -1];

#endif
