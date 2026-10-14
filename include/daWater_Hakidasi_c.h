#ifndef DAWATER_HAKIDASI_C_H
#define DAWATER_HAKIDASI_C_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daWater_Hakidasi_c. The reconstructed
 * factory daWater_Hakidasi_c_classInit (historical alias
 * JetStream_Spawn) constructs it for the WATER_HAKIDASI
 * registry profile.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct daWater_Hakidasi_c : dEnemyBase_c {
    /* What mState points at. Behavior calls the handler at +0x08 through it and
       nothing else in the tree reads the rest, so only that field is evidenced.
       daPropeller_Heyho_Fire_c::State (include/daPropeller_Heyho_Fire_c.h) puts
       its per-frame handler at the same +0x08; there an install helper also
       calls an int-returning mInit at 0x00, which this file never reads. */
    struct State {
        u8  pad_00[0x8];
        void (daWater_Hakidasi_c::*mMain)();   /* 0x08 */
    };

    dCcAc_c mdCcAc_c;/* 0x110 */
    dBgCh_Actr mWithMeshClsn;       /* 0x144 */
    State *mState;                  /* 0x300 -- run every frame by Behavior */
    s32 mParticle;                  /* 0x304 -- Particle::System::New handle, re-fed every frame */
    u8  pad_308[0x4];
    s32 mRingsPassed;                  /* 0x30c -- rings passed in order, 0..5, then 0xa once the STAR spawns */
    s32 mRewardTimer;               /* 0x310 -- frames until the STAR spawns; counts while mRingsPassed is 5 */
    /* Both are decoded out of the spawn word (fBase_c::param1) by InitResources
       and never read back anywhere in the tree, so what they select is unknown:
       0x314 takes nibble 3 (param1 >> 12 & 0xf), 0x318 takes bit 0 but only when
       the low nibble is 0 or 1. Deliberately left unk_ -- see
       notes/enemy-leaf-provenance.md. */
    s32 unk_314;                      /* 0x314 */
    s32 unk_318;                      /* 0x318 */

    /* --- vtable --- */
    virtual ~daWater_Hakidasi_c();

    virtual s32 Behavior();
    virtual s32 CleanupResources();
    virtual s32 InitResources();
    virtual void OnPendingDestroy();
    virtual s32 Render();

    /* Helpers taking this (S33): the ROM address is the method name. */
    int func_ov064_021193b4();
    int func_ov064_021197fc();
    void func_ov064_0211987c();

    /* The tail runs to the ROM's 0x378: daWater_Hakidasi_c_classInit calls
       fBase_c::operator new(0x378), read off the retail instruction. A span is
       only a LOWER BOUND. func_ov064_021197fc zeroes all of it below. */
    dActor_c *mPassedRing;            /* 0x31c -- set by a spawned WATER_RING (daWater_Ring_c) when the player passes through it */
    u32 mSpawnedIDs[20];            /* 0x320 -- uniqueIDs of the WATER_RINGs it spawned, a 20-entry ring buffer */
    s32 mSpawnedHead;               /* 0x370 -- next ring slot to write */
    s32 mMatchedSlot;               /* 0x374 -- buffer slot where mPassedRing was found */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daWater_Hakidasi_c_size_must_be_0x378[sizeof(daWater_Hakidasi_c) == 0x378 ? 1 : -1];
#endif

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/dExtShadowModel_c.h. */
struct daWater_Hakidasi_c {
    u8  pad_000[0x8];
    u32 param1;            /* 0x008 */
    u8  pad_00c[0x80];
    /* 0x008..0x10f is the fBase_c/dBase_c/dActor_c/dEnemyBase_c chain restated flat.
       Every name below is dActor_c.h's own (fBase_c.h's for param1); nothing here
       is an independent claim, and the C++ branch above inherits them properly. */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;                  /* 0x0b0 */
    s32 mClipOffsetY;            /* 0x0b4 */
    s32 mClipRadius;             /* 0x0b8 */
    s32 mClipDistance;           /* 0x0bc */
    s32 mFarDistance;            /* 0x0c0 */
    u8  mClipResult;             /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 mDeathTableID;           /* 0x0ce */
    u8  pad_0d0[0x30];
    u8  mStateTimer;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mdCcAc_c;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    void *mState;                /* 0x300 */
    u8  pad_304[0x10];
    s32 unk_314;            /* 0x314 */
    s32 unk_318;            /* 0x318 */
};

#endif /* __cplusplus */

#endif /* DAWATER_HAKIDASI_C_H */
