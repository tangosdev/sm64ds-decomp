#ifndef DAC_JUGEM_C_H
#define DAC_JUGEM_C_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN11daC_Jugem_cD1Ev` destroys each member, and
 * the factory at 0x0212ed54 (historical alias `LakituBro_Spawn`) constructs
 * the same types at the same offsets before storing `_ZTV11daC_Jugem_c`.
 * Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 ModelAnim                  0x64    -> 0x174
 *     0x174 ModelAnim                  0x64    -> 0x1d8
 *     0x1d8 TextureSequence            0x14    -> 0x1ec
 *     0x1ec state pointer               0x4    -> 0x1f0
 *     0x1f0 ShadowModel                0x28    -> 0x218
 *     0x218 ShadowModel                0x28    -> 0x240
 *     0x240 shadow matrix              0x30    -> 0x270
 *     0x270 shadow matrix              0x30    -> 0x2a0
 *     0x2a0 Player* (who he talks to)   0x4    -> 0x2a4
 *
 * Plain fields (not constructed, so neither witness covers them): 0x1ec
 * state-table pointer (func_ov085_0212e728, Behavior); 0x240/0x270 shadow
 * matrices (the DropShadowRadHeight arguments); 0x2a0 the Player Lakitu
 * talks to (set in func_ov085_0212e310).
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_16c = ModelAnim.speed
 *
 * SIZE IS THE ROM'S OWN: that same factory calls
 * `fBase_c::operator new(744)` -- 0x2e8 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 *
 * SM64DS proves this class as daC_Jugem_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daC_Jugem_c_classInit at 0x0212ed54 (historical alias LakituBro_Spawn)
 * allocates 0x2e8 and installs this class's cartridge vtable. It backs the
 * C_JUGEM registry profile, whose descriptor at 0x02130320 is reconstructed
 * as g_profile_C_JUGEM.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "TextureSequence.h"

struct daC_Jugem_c : dEnemyBase_c {
    /* Not Matrix4x3: this TU sees math/Matrix.h first, and that spelling's
       Vector3 destructor would reorder ~daC_Jugem_c. The copies are 12 words. */
    struct ShadowMat { s32 m[12]; };

    ModelAnim                    mModelAnim1;           /* 0x110 */
    ModelAnim                    mModelAnim2;           /* 0x174 */
    TextureSequence              mTextureSequence;      /* 0x1d8 */
    void                        *mState;                /* 0x1ec */
    ShadowModel                  mShadowModel1;         /* 0x1f0 */
    ShadowModel                  mShadowModel2;         /* 0x218 */
    ShadowMat                    mShadowMat1;           /* 0x240 */
    ShadowMat                    mShadowMat2;           /* 0x270 */
    Player                      *mTalkPlayer;           /* 0x2a0 */
    s32                          mTargetX;              /* 0x2a4 */
    s32                          mTargetY;              /* 0x2a8 */
    s32                          mTargetZ;              /* 0x2ac */
    s32                          mCamLookX;             /* 0x2b0 */
    s32                          mCamLookY;             /* 0x2b4 */
    s32                          mCamLookZ;             /* 0x2b8 */
    s32                          mCamPosX;              /* 0x2bc */
    s32                          mCamPosY;              /* 0x2c0 */
    s32                          mCamPosZ;              /* 0x2c4 */
    s32                          mTimer;                /* 0x2c8 */
    s32                          unk_2cc;               /* 0x2cc */
    s32                          unk_2d0;               /* 0x2d0 */
    s16                          mSavedAngleY;          /* 0x2d4 */
    u8                           pad_2d6[0x2];
    s32                          mTalkStep;             /* 0x2d8 */
    u8                           unk_2dc;               /* 0x2dc */
    u8                           pad_2dd[0x3];
    s32                          unk_2e0;               /* 0x2e0 */
    u32                          mSfxHandle;            /* 0x2e4 */

    /* --- vtable --- */
    virtual ~daC_Jugem_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daC_Jugem_c_size_must_be_0x2e8[sizeof(daC_Jugem_c) == 0x2e8 ? 1 : -1];
#endif

#endif /* DAC_JUGEM_C_H */
