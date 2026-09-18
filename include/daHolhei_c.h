#ifndef DAHOLHEI_C_H
#define DAHOLHEI_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

/* Chuckya (HOLHEI 190) -- ov062/daHolhei_c.
 *
 * RTTI ov062:0x0211d9ac is the NUL-terminated string 10daHolhei_c;
 * _ZTI10daHolhei_c at 0x0211d9a0 points its name word there and its base
 * at dEnemyBase_c. The tree used to carry this class under the coined
 * English name Chuckya, which appears nowhere in the cartridge.
 *
 * Factory daHolhei_c_classInit (historical alias Chuckya_Spawn) allocates
 * 0x438 bytes; that is this class's size. Member NAMES below are from this
 * TU's own methods. dCcAc_c::Init / dBgCh_Actr::Init keep Fix12<int> by
 * value in their ROM mangled names; a method call homes those to the stack
 * (notes/mwccarm-codegen.md 6az), so those two stay scalar-slot wrappers.
 *
 * SM64DS RTTI names the implementation daHolhei_c. The reconstructed
 * factory daHolhei_c_classInit (historical alias Chuckya_Spawn) installs
 * this class's cartridge vtable; the reconstructed profile global
 * g_profile_HOLHEI (historical alias Chuckya_SpawnInfo) is its registry
 * descriptor. Exact original SM64DS member spellings are not preserved.
 */

struct daHolhei_c : dEnemyBase_c {
    dCcAc_c     mdCc_c;        /* 0x110 */
    dBgCh_Actr  mMeshClsn;     /* 0x144 */
    ModelAnim   mModel;        /* 0x300 */
    /* State pointer between ModelAnim (ends 0x364) and ShadowModel (0x368).
       Behavior dispatches the PMF at *mState; InitResources seeds it via
       daHolhei_c_ChangeState onto data_ov062_0211dee0. */
    void       *mState;        /* 0x364 */
    ShadowModel mShadowModel;  /* 0x368 */
    /* 0x390..0x3c0: func_ov062_02116dbc / 02116d28 copy IDENTITY_MATRIX
       then pos>>3 and hand the result to DropShadowRadHeight. */
    u8          pad_390[0x30];
    s32         mHomePosX;     /* 0x3c0 -- InitResources copies mPos here */
    s32         mHomePosY;     /* 0x3c4 */
    s32         mHomePosZ;     /* 0x3c8 */
    /* 0x3cc..0x3d8: func_ov062_021165e8 stores the closest player's pos
       when the charge sub-step starts. */
    s32         mChasePosX;    /* 0x3cc */
    s32         mChasePosY;    /* 0x3d0 */
    s32         mChasePosZ;    /* 0x3d4 */
    /* Own previous-position triple, not dActor_c::mPrevPos at 0x068.
       Behavior rewinds mPos from these on a cliff/bound trip and
       republishes them after terrain. */
    s32         mPrevPosX;     /* 0x3d8 */
    s32         mPrevPosY;     /* 0x3dc */
    s32         mPrevPosZ;     /* 0x3e0 */
    /* Behavior sets 1 when IsGoingOffCliff or the level fence trips, 0
       when the walk is clear. State handlers also read it. */
    u8          mEdgeStop;     /* 0x3e4 */
    u8          unk_3e5;       /* 0x3e5 -- func_ov062_021164e8 grab-anim flag */
    u16         unk_3e6;       /* 0x3e6 -- Behavior DecIfAbove0_Short */
    u16         unk_3e8;       /* 0x3e8 -- Behavior DecIfAbove0_Short */
    u8          pad_3ea[0xe];
    void       *mHeld;         /* 0x3f8 -- grabbed actor; Render/Behavior +0xc8 */
    /* 0x3fc..0x42c: func_ov062_02116e80 writes the hold matrix and stores
       its address at this+0xc8. */
    u8          pad_3fc[0x30];
    /* Behavior zeroes these every frame; func_ov062_02116edc homes them
       toward data_ov062_0211df10 and hands them to UpdateCarry. */
    s32         mCarryOffsX;   /* 0x42c */
    s32         mCarryOffsY;   /* 0x430 */
    s32         mCarryOffsZ;   /* 0x434 */

    /* INLINE IS LOAD-BEARING. Out-of-line, mwccarm emits D0 before D1 plus
       a homeless D2; the cartridge has D1 at 0x02115ee0 then D0 at 0x02115f28
       and no D2. */
    virtual ~daHolhei_c() {}

    virtual s32 OnAimedAtWithEgg(); /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daHolhei_c_size_must_be_0x438[sizeof(daHolhei_c) == 0x438 ? 1 : -1];
#endif

#endif /* DAHOLHEI_C_H */
