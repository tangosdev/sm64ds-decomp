#ifndef DAOWL_C_H
#define DAOWL_C_H

#include "types.h"

/* The owl actor (ROM RTTI name daOwl_c, registry profile OWL). It hovers near
 * a perch, talks to the player when approached, and afterwards can pick the
 * player up and carry it (the carry state pins the actor to the rider's
 * matrix). Five states drive it; see the table in d_a_owl.cpp.
 *
 * Derives from dEnemyBase_c, and the class's own destructor is what proves it:
 * `_ZN7daOwl_cD1Ev` stores this vtable, destroys the four members below in
 * reverse declaration order, then calls `dEnemyBase_c::~dEnemyBase_c`.
 *
 * The four members close exactly on each other, four independent
 * confirmations of one layout:
 *
 *     0x110 dCcAcPos_c  0x40   -> 0x150
 *     0x150 dBgCh_Actr  0x1bc  -> 0x30c
 *     0x30c ModelAnim   0x64   -> 0x370
 *     0x370 dExtShadowModel_c 0x28   -> 0x398
 *
 * and dEnemyBase_c's own 0x110 closes exactly on the first of them.
 *
 * Typing them absorbed four markers that were their insides:
 *   - unk_128 = mdCcAcPos_c.flags  (dCc_c +0x18)
 *   - mAnimation = the ModelAnim's dExtFrameCtrl_c base (+0x50)
 *   - unk_364 = that dExtFrameCtrl_c's currFrame (+0x08); Behavior reads it as
 *     `>> 12`, the integer frame of a 20.12 fixed-point count
 *   - unk_368 = that dExtFrameCtrl_c's speed (+0x0c); Behavior copies mAnimSpeed
 *     into it, and InitResources sets mAnimSpeed to 0x1000, which is 1.0
 *
 * Size is the ROM's own, not a rounded-up field span: `daOwl_c_classInit`
 * calls `fBase_c::operator new(1016)` -- 0x3f8 -- and stores `_ZTV7daOwl_c`.
 * The fields below span to 0x3f4; the rest is trailing space no source reads.
 *
 * daOwl_c_classInit at 0x02136798 allocates 0x3f8 and installs the vtable. It
 * backs the OWL registry profile, whose descriptor at 0x02136a34 is
 * reconstructed as g_profile_OWL. The factory and profile spellings are
 * source-style names, not recovered SM64DS symbols.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

struct daOwl_c : dEnemyBase_c {
    /* What mCurrentState points at: two pointers to member function, run by
       func_ov094_02136188 (enter, once, on entry) and Behavior (main, every
       frame). ov094's __sinit copies each state from a pair of PMF constants
       in .data into data_ov094_02136b30..02136b70. */
    struct State {
        int (daOwl_c::*mEnter)();   /* 0x00 */
        int (daOwl_c::*mMain)();    /* 0x08 */
    };

    dCcAcPos_c mdCcAcPos_c;           /* 0x110 */
    dBgCh_Actr mWithMeshClsn;         /* 0x150 */
    ModelAnim mModelAnim;             /* 0x30c */
    dExtShadowModel_c mShadowModel;         /* 0x370 */
    Matrix4x3 mShadowMat;             /* 0x398 -- fed to DropShadowRadHeight */
    State *mCurrentState;             /* 0x3c8 */
    struct Player *mRider;            /* 0x3cc -- the player being carried, else 0 */
    struct Player *mTalkTarget;       /* 0x3d0 -- the player it faces and talks to */
    u8  mTalkState;                   /* 0x3d4 -- 0 idle, 1 talking, 2 talk finished */
    u8  pad_3d5[3];
    s32 mAnchorX;                     /* 0x3d8 -- hover anchor, returned to after a carry */
    s32 mAnchorY;                     /* 0x3dc */
    s32 mAnchorZ;                     /* 0x3e0 */
    u8  mOpacity;                     /* 0x3e4 -- 0..0x1f, passed to ApplyOpacity */
    u8  pad_3e5[3];
    s32 unk_3e8;                      /* 0x3e8 -- per-state scratch, see d_a_owl.cpp */
    s16 mWanderAngleY;                /* 0x3ec -- heading the hover state steers to */
    u8  pad_3ee[2];
    Fix12i mAnimSpeed;                /* 0x3f0 -- copied into mModelAnim.speed */
    u32 mSoundHandle;                 /* 0x3f4 -- Sound::PlayLong's returned handle */

    /* --- vtable --- */
    virtual ~daOwl_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* Non-virtual. The first parameter was this actor, so each is a member.
       The cartridge keeps no English spelling; the ROM address is the name. */
    void func_ov094_021357a4();
    int func_ov094_021358b4();
    int func_ov094_0213598c();
    int func_ov094_021359d8();
    int func_ov094_02135bd4();
    int func_ov094_02135c28();
    int func_ov094_02135e64();
    int func_ov094_02135ee0();
    int func_ov094_02135fe0();
    int func_ov094_02136024();
    int func_ov094_02136150();
    int func_ov094_02136188(State *state);
    void func_ov094_021361d8();
    void func_ov094_021362e0();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daOwl_c_size_must_be_0x3f8[sizeof(daOwl_c) == 0x3f8 ? 1 : -1];
#endif

#endif /* DAOWL_C_H */
