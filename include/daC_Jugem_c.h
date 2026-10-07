#ifndef DAC_JUGEM_C_H
#define DAC_JUGEM_C_H

#include "types.h"

/* Derives from dEnemyBase_c. Two independent witnesses agree on the layout:
 * the class's own destructor destroys each member, and the factory at
 * 0x0212ed54 (historical alias LakituBro_Spawn) constructs the same types at
 * the same offsets before storing _ZTV11daC_Jugem_c.
 *
 *     0x110 ModelAnim                  0x64    -> 0x174
 *     0x174 ModelAnim                  0x64    -> 0x1d8
 *     0x1d8 TextureSequence            0x14    -> 0x1ec
 *     0x1ec state-record pointer        0x4    -> 0x1f0
 *     0x1f0 dExtShadowModel_c          0x28    -> 0x218
 *     0x218 dExtShadowModel_c          0x28    -> 0x240
 *     0x240 shadow matrix              0x30    -> 0x270
 *     0x270 shadow matrix              0x30    -> 0x2a0
 *     0x2a0 Player* (who he talks to)   0x4    -> 0x2a4
 *
 * The factory calls fBase_c::operator new(744), so 0x2e8 is this class's
 * sizeof. SM64DS proves it as daC_Jugem_c through RTTI, allocation size and
 * vtable identity; the factory and profile spellings are reconstructed
 * source-style names, not recovered SM64DS symbols.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dExtShadowModel_c.h"
#include "TextureSequence.h"

struct daC_Jugem_c : dEnemyBase_c {
    /* Not Matrix4x3: this TU sees math/Matrix.h first, and that spelling's
       Vector3 destructor would reorder ~daC_Jugem_c. The copies are 12 words. */
    struct ShadowMat { s32 m[12]; };

    /* The state machine this class drives itself with: twelve records, each
       an {enter, run} pair of pointers-to-member, built by the module sinit.
       SetState installs one and runs its enter; Behavior calls the run. */
    typedef int (daC_Jugem_c::*StateFn)();

    ModelAnim                    mModelAnim1;           /* 0x110 */
    ModelAnim                    mModelAnim2;           /* 0x174 */
    TextureSequence              mTextureSequence;      /* 0x1d8 */
    StateFn                     *mState;                /* 0x1ec */
    dExtShadowModel_c            mShadowModel1;         /* 0x1f0 */
    dExtShadowModel_c            mShadowModel2;         /* 0x218 */
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
    s32                          mAuxCounter;           /* 0x2cc -- per-state scratch */
    s32                          mVariant;              /* 0x2d0 -- spawn-param variant; picks the init path and the shadow update */
    s16                          mSavedAngleY;          /* 0x2d4 */
    u8                           pad_2d6[0x2];
    s32                          mTalkStep;             /* 0x2d8 */
    u8                           mHidden;               /* 0x2dc -- Render returns early while set */
    u8                           pad_2dd[0x3];
    s32                          unk_2e0;               /* 0x2e0 -- written once, never read */
    u32                          mSfxHandle;            /* 0x2e4 */

    /* --- vtable --- */
    virtual ~daC_Jugem_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* --- state machine --- */
    int SetState(StateFn *record);

    int StateHoverInit();        /* record 0x021307d0: the ambient variant-0 state */
    int StateHoverMain();
    int StateIntroInit();        /* record 0x021307a0: the camera sweep */
    int StateIntroMain();
    int StateHiddenInit();       /* record 0x021307e0: hidden, waits for the player */
    int StateHiddenMain();
    int StateFlyToPlayerInit();  /* record 0x02130800 */
    int StateFlyToPlayerMain();
    int StateTalkInit();         /* record 0x02130810: the message 0x182 exchange */
    int StateTalkMain();
    int StateLeaveInit();        /* record 0x02130830: flies off, destructs */
    int StateLeaveMain();
    int StateArriveInit();       /* record 0x02130790: the variant-1 entry */
    int StateArriveMain();
    int StateTurnInit();         /* record 0x021307b0 */
    int StateTurnMain();
    int StateGuideInit();        /* record 0x021307c0 */
    int StateGuideMain();
    int StateBobInit();          /* record 0x021307f0 */
    int StateBobMain();
    int StateApproachInit();     /* record 0x02130820 */
    int StateApproachMain();

    void UpdateShadow();         /* cutscene variant: one shadow at own position */
    void UpdateShadowPlayer();   /* ambient variant: own shadow plus one ahead of the player */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daC_Jugem_c_size_must_be_0x2e8[sizeof(daC_Jugem_c) == 0x2e8 ? 1 : -1];
#endif

#endif /* DAC_JUGEM_C_H */
