#ifndef DAWBM_C_H
#define DAWBM_C_H

#include "types.h"

/* The water bomb (WATERBOMB profile). The cartridge's RTTI names the class
 * daWbm_c: _ZTS at ov098 0x0213c734 reads "7daWbm_c", and the _ZTI at
 * 0x0213c740 names the vtable at 0x0213c770 as this class's. The tree called
 * it WaterBomb until then.
 *
 * Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN7daWbm_cD1Ev` destroys each member, and
 * `daWbm_c_classInit` constructs the same types at the same offsets before
 * storing `_ZTV7daWbm_c`. Everything below 0x110 belongs to dEnemyBase_c and
 * dActor_c and is inherited.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dCcAc_c         0x34    -> 0x144
 *     0x144 dBgCh_Actr      0x1bc   -> 0x300
 *     0x300 Model           0x50    -> 0x350
 *     0x350 ShadowModel     0x28    -> 0x378
 *     0x378 Matrix4x3       0x30    -> 0x3a8
 *
 * SIZE IS THE ROM'S OWN: `daWbm_c_classInit` calls
 * `fBase_c::operator new(972)` -- 0x3cc -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 *
 * mVariant is param1 & 0xf. 0 is an invisible dropper: it waits for a player
 * to walk beneath it and spawns a variant-1 bomb above them. 1 is the falling
 * bomb, which squashes and hops toward the closest player, and bursts into
 * five variant-2 fragments on its third landing or when it hits a player. 2
 * is a fragment at half scale. The per-variant roles
 * and the squash field names are inferred from the bodies in
 * src/actors/daWbm_c.cpp, not recovered names.
 *
 * The reconstructed factory daWbm_c_classInit (historical alias
 * WaterBomb_Spawn) constructs it for the WATERBOMB registry profile.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct daWbm_c : dEnemyBase_c {
    dCcAc_c                      mdCcAc_c;              /* 0x110 */
    dBgCh_Actr                   mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x350 */
    Matrix4x3                    mShadowMat;            /* 0x378 */
    /* Where it was placed. InitResources writes these and nothing in the
       class reads them back. */
    s32                          mSpawnPosX;            /* 0x3a8 */
    s32                          mSpawnPosY;            /* 0x3ac */
    s32                          mSpawnPosZ;            /* 0x3b0 */
    s16                          unk_3b4;               /* 0x3b4 -- zeroed, never read */
    u8                           mBounceCount;          /* 0x3b6 */
    u8                           pad_3b7[0x1];
    /* The landing squash: mSquashScale is eased toward a target by
       mSquashSpeed, which decays every frame; mBouncePhase steps through
       squash, stretch and airborne. */
    s32                          mSquashSpeed;          /* 0x3b8 */
    s32                          mSquashScale;          /* 0x3bc */
    s32                          mBouncePhase;          /* 0x3c0 */
    s32                          mState;                /* 0x3c4 */
    s32                          mVariant;              /* 0x3c8 */

    /* Inline and empty, so Behavior is the key function and its TU emits the
       vtable and RTTI. Written out of line, mwccarm emits D0 ahead of D1 (the
       cartridge has D1 first) plus a D2 with no ROM home. Keep the brace on
       this line: tools/check_header_offsets.py needs it. */
    virtual ~daWbm_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    void Land();
    void Burst();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daWbm_c_size_must_be_0x3cc[sizeof(daWbm_c) == 0x3cc ? 1 : -1];
#endif

#endif /* DAWBM_C_H */
