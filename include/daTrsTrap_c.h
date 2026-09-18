/* Big Boo's Haunt animated-furniture traps: the rising staircase (KAIDAN),
 * the tilting trapdoor (TERESAPIT), the sliding bookshelf (BOOKSHELF) and the
 * spinning merry-go-round (MERRYGOROUND). One class, four registry profiles --
 * every factory allocates 852 = 0x354 and stores _ZTV11daTrsTrap_c
 * (ov063:0x0211ea88), which is what proves a single class rather than four.
 * mIndex (from the spawn profile's actor id) selects the resource row and the
 * per-frame state body; the bodies themselves live in unnamed ov063 shards and
 * are reached through the .bss dispatch table, not through this header. */
#ifndef DATRSTRAP_C_H
#define DATRSTRAP_C_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"
#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daTrsTrap_c : dActor_c {
    u32 pad_0d0;                 /* 0x0d0 -- dActor_c closes at 0xd0, mModel opens at 0xd4 */
    Model mModel;                /* 0x0d4 */
    /* Set by the mesh-collision callback (func_ov063_0211d270) while the player
       (actor 0xbf) stands on the trapdoor; cleared every Behavior frame. The
       trapdoor state steers off its position, the merry-go-round off nullness. */
    dActor_c *mStandingActor;    /* 0x124 */
    /* The KAIDAN step this step follows: the master holds 0 and each child the
       master's uniqueID, resolved through dActor_c::FindWithID by the rise
       state. */
    u32 mParentUniqueID;         /* 0x128 */
    s32 mHomePosX;               /* 0x12c -- placement snapshot; the rise/slide states move mPos* off it */
    s32 mHomePosY;               /* 0x130 */
    s32 mHomePosZ;               /* 0x134 */
    s16 mAngVelX;                /* 0x138 -- per-frame tilt, added to mAngleX by the trapdoor state */
    s16 mAngVelY;                /* 0x13a -- per-frame spin, added to mAngleY by the merry-go-round state */
    s16 mAngVelZ;                /* 0x13c -- zeroed with the pair; no state reads it yet */
    u8  pad_13e[0x2];
    /* 0 KAIDAN, 1 TERESAPIT, 2 BOOKSHELF, 3 MERRYGOROUND. Indexes the model,
       collision and CLPS rows and the Behavior dispatch table. */
    s32 mIndex;                  /* 0x140 */
    s32 mRiseProgress;           /* 0x144 -- KAIDAN rise counter, +8/frame toward the per-step limit */
    u32 mSoundHandle;            /* 0x148 -- Sound::PlayLong in/out handle for the slide/spin loop */
    u16 mStateTimer;             /* 0x14c -- frames in mState; Behavior bumps it, zeroes it on change */
    u16 mSoundTimer;             /* 0x14e -- Sound::PlaySecretSound progress counter */
    u8  mState;                  /* 0x150 */
    /* (param1 >> 8) & 3: for KAIDAN the step index (0 master, 1-2 children),
       selecting the rise limit; the secret-chime path is index 2. */
    u8  mStepIndex;              /* 0x151 */
    u8  mInitLatch;              /* 0x152 -- merry-go-round skips occupancy once, then latches this */
    /* Merry-go-round occupancy: a rider stands on the mesh or the area is
       showing. Written every frame; no reader recovered yet. */
    u8  mOccupiedFlag;           /* 0x153 */
    u8  pad_154;                 /* 0x154 -- zeroed by InitResources, no reader recovered */
    /* KAIDAN release: the Boo's death path (ov063:0x02118b98) finds the master
       step and raises this, and the rise state waits on it. */
    u8  mTriggered;              /* 0x155 */
    u8  mVisible;                /* 0x156 -- Render skips the model while clear */
    /* Bookshelf trigger bits: low three ORed in by the book-switch callback
       (func_ov063_0211cae8), bit 3 set once the player is behind the shelf.
       All three switches (0x7) starts the slide. */
    u8  mBookFlags;              /* 0x157 */
    s8  mSavedAreaId;            /* 0x158 -- bookshelf parks mAreaId here while forcing -1 */
    u8  pad_159[0x3];
    dBgW_KcMbg mMovingMeshCollider; /* 0x15c */
    Matrix4x3 mClsnMat;          /* 0x324 -- rebuilt from placement every frame, handed to SetFile/Transform */

    /* --- vtable ---
     * Overrides of fBase_c virtuals, so each takes the base's slot whatever the
     * order here; the destructor stays first-declared because it is the ABI key
     * function. It is defined out of line in the dtors file, so that TU -- not
     * the merged method TU -- is the one whose object carries the vtable. */
    virtual ~daTrsTrap_c();          /* slots 16 (D1), 17 (D0) */
    virtual s32 InitResources();     /* slot  0 -- ov063:0x0211cf00 */
    virtual s32 CleanupResources();  /* slot  3 -- ov063:0x0211cdec */
    virtual s32 Behavior();          /* slot  6 -- ov063:0x0211ce74 */
    virtual s32 Render();            /* slot  9 -- ov063:0x0211ce34 */
    virtual void OnPendingDestroy(); /* slot 12 -- ov063:0x0211ce30 */

    /* Leaf adapter until fBase_c::operator new(unsigned long) lands (#2570).
       `return new daTrsTrap_c()` then routes through the retail allocator. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daTrsTrap_c_size_must_be_0x354[sizeof(daTrsTrap_c) == 0x354 ? 1 : -1];
#endif

#endif
