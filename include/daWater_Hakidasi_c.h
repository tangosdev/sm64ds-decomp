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

#include "dEnemyBase_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct daWater_Hakidasi_c : dEnemyBase_c {
    /* SetState stores the state pointer and runs mInit; Behavior runs
       mMain every frame. The one state is InitState0 / State0. */
    struct State {
        int (daWater_Hakidasi_c::*mInit)(); /* 0x00 */
        int (daWater_Hakidasi_c::*mMain)(); /* 0x08 */
    };

    dCcAc_c mdCcAc_c;         /* 0x110 */
    dBgCh_Actr mWithMeshClsn; /* 0x144 */
    State *mState;            /* 0x300 -- run every frame by Behavior */
    s32 mParticle;            /* 0x304 -- Particle::System::New handle, re-fed every frame */
    u8 pad_308[0x4];
    s32 mRingsPassed; /* 0x30c -- rings passed in order, 0..5, then 0xa once the STAR spawns */
    s32 mRewardTimer; /* 0x310 -- frames until the STAR spawns; counts while mRingsPassed is 5 */
    /* Nibble 3 of param1 (InitResources). State0 spawns the
       STAR (actor 0xb2) with param1 mStarID | 0x40, the spelling
       daObjBk_Kabe_c uses for the same Spawn. Was unk_314. */
    s32 mStarID; /* 0x314 */
    /* Bit 0 of param1 when its low nibble is 0 or 1, else 0 (InitResources).
       Set, State0 only re-feeds particle 0x7b: no WATER_RING
       spawns, no ring count, no STAR. Was unk_318. */
    s32 mNoRings; /* 0x318 */

    /* --- vtable --- */
    virtual ~daWater_Hakidasi_c();

    virtual s32 Behavior();
    virtual s32 CleanupResources();
    virtual s32 InitResources();
    virtual void OnPendingDestroy();
    virtual s32 Render();

    /* --- non-virtual --- */
    int State0();               /* ov064 0x021193b4 */
    int InitState0();           /* ov064 0x021197fc */
    int SetState(State *);      /* ov064 0x0211982c */
    void func_ov064_0211987c(); /* ov064 0x0211987c, empty */

    /* The tail runs to the ROM's 0x378: daWater_Hakidasi_c_classInit calls
       fBase_c::operator new(0x378), read off the retail instruction. A span is
       only a LOWER BOUND. InitState0 zeroes all of it below. */
    dActor_c *
        mPassedRing; /* 0x31c -- set by a spawned WATER_RING (daWater_Ring_c) when the player passes through it */
    u32 mSpawnedIDs
        [20];         /* 0x320 -- uniqueIDs of the WATER_RINGs it spawned, a 20-entry ring buffer */
    s32 mSpawnedHead; /* 0x370 -- next ring slot to write */
    s32 mMatchedSlot; /* 0x374 -- buffer slot where mPassedRing was found */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daWater_Hakidasi_c_size_must_be_0x378[sizeof(daWater_Hakidasi_c) == 0x378 ? 1 : -1];
#endif

#endif /* DAWATER_HAKIDASI_C_H */
