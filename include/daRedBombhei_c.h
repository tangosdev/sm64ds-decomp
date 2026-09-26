#ifndef DAREDBOMBHEI_C_H
#define DAREDBOMBHEI_C_H

#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dActor_c.h"

struct Player;

/* Bob-omb Buddy, the pink Bob-omb that opens a level's cannon (profile
 * RED_BOMBHEI). ROM RTTI daRedBombhei_c: _ZTI14daRedBombhei_c at ov084
 * 0x021309f4 names the string "14daRedBombhei_c" at 0x02130a00 and its base
 * as _ZTI8dActor_c; the vtable's address point is 0x02130a38.
 *
 * daRedBombhei_c_classInit at 0x0212d200 allocates 0x1f0, runs dActor_c's
 * C2, stores the vtable and constructs the three members below. Only the
 * slots declared here differ from _ZTV8dActor_c: InitResources (0),
 * CleanupResources (3), Behavior (6), Render (9) and D1/D0 (16/17).
 *
 * daRedBombhei_c_classInit and g_profile_RED_BOMBHEI (0x02130a14) are
 * reconstructed source-style names -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Historical aliases: BobOmbBuddy (the class),
 * BobOmbBuddy_Spawn, BobOmbBuddy_SpawnInfo.
 */
struct daRedBombhei_c : dActor_c {
    u8          pad_0d0[0x4];
    dCcAc_c     mdCcAc_c;           /* 0x0d4 -- talk trigger; a player hit starts the talk */
    ModelAnim   mModelAnim;         /* 0x108 */
    ShadowModel mShadowModel;       /* 0x16c */
    Player     *mTalkPlayer;        /* 0x194 -- the player that walked into mdCcAc_c */
    u32         mShutterID;         /* 0x198 -- uniqueID of this level's closed cannon shutter, 0 if none */
    Matrix4x3   mShadowMat;         /* 0x19c */
    Vector3     mSavedCamLookAt;    /* 0x1cc -- camera, captured when the cannon cutscene starts */
    Vector3     mSavedCamPos;       /* 0x1d8 */
    s32         mState;             /* 0x1e4 -- index into the state table, see the TU */
    u8          mCutsceneStep;      /* 0x1e8 -- step of the cannon-opening cutscene */
    u8          pad_1e9;
    u8          mCannonOpened;      /* 0x1ea -- picks the "already opened" line next time */
    u8          mMsgPage;           /* 0x1eb -- message pages advanced this talk */
    u16         mPrevMsgPageState;  /* 0x1ec -- last frame's message page state */
    u8          mMsgHint;           /* 0x1ee -- last frame's value of data_0209f284 */
    u8          pad_1ef;

    /* Inline is load-bearing: out of line emits D0 before D1. */
    virtual ~daRedBombhei_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daRedBombhei_c_size_must_be_0x1f0[
    sizeof(daRedBombhei_c) == 0x1f0 ? 1 : -1];
#endif

#endif /* DAREDBOMBHEI_C_H */
