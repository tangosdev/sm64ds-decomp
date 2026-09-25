/* daEykn_c: Mr. I, the eye that turns to watch a player and shoots at it.
 * Offsets and widths are pinned by the bytes. The names of the fields from
 * 0x1b4 on, and of the non-virtual methods, are coined from how
 * src/actors/daEykn_c.cpp uses them; the ROM records none of them. */
#ifndef DAEYKN_C_H
#define DAEYKN_C_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "TextureSequence.h"

/* The ROM's RTTI name for this class is daEykn_c: _ZTS8daEykn_c at ov071
 * 0x02122cd8, and _ZTI8daEykn_c at 0x02122ce4 is an __si_class_type_info whose
 * base is _ZTI8dActor_c, so the base is dActor_c. The vtable is _ZTV8daEykn_c
 * (address point 0x02122d30); the tree's old coined name MrI only aliased
 * _ZTV3MrI to that same address, and the alias is gone. */
struct daEykn_c : dActor_c {
#ifdef __cplusplus
    struct State;
#endif
    /* dActor_c ends at 0x0d0; the derived payload begins at 0x0d4. */
    u8 pad_0d0[0x4];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* TextureSequence member. The cartridge's own ~daEykn_c calls _ZN15TextureSequenceD1Ev
       at +0x138 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    TextureSequence mTextureSequence;            /* 0x138 */
    /* ShadowModel member. The cartridge's own ~daEykn_c calls _ZN11ShadowModelD1Ev at +0x14c
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel;            /* 0x14c */
    /* dCcAcPos_c member. The cartridge's own ~daEykn_c calls _ZN10dCcAcPos_cD1Ev at +0x174
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    dCcAcPos_c mdCcAcPos_c;            /* 0x174 */
    /* The shadow's Matrix4x3, spelled flat: a Matrix4x3 member holds a Vector3,
       whose inline destructor reorders this class's D1, D0 and D2 output. */
    s32 mShadowMat[12];        /* 0x1b4 -- identity; [9..11] track the position */
    const State *mState;       /* 0x1e4 -- current row of the state table */
    s32 mStateID;              /* 0x1e8 -- 0 wait, 1 attack, 2 die; set by each init */
    Player *mTarget;           /* 0x1ec -- the player being watched, or null */
    s32 mScale;                /* 0x1f0 -- body scale; shadow radius and death swell */
    s32 mCircleAngle;          /* 0x1f4 -- yaw turned while following one direction */
    s32 mDeathSpinAngle;       /* 0x1f8 -- yaw turned while dying, per sound cue */
    s32 mWobbleAmp;            /* 0x1fc -- pitch wobble amplitude while dying */
    s32 mShadowHeight;         /* 0x200 */
    u32 mParticleID0;          /* 0x204 -- particle system 0x13a */
    u32 mParticleID1;          /* 0x208 -- particle system 0x13b */
    s16 mTurnRefAngleY;        /* 0x20c -- last frame's yaw */
    s16 mDeathSpinSpeed;       /* 0x20e -- target yaw speed while dying */
    u16 mWobblePhase;          /* 0x210 */
    u8  unk_212;               /* 0x212 -- only ever written (0xf0) */
    u8  mShotTimer;            /* 0x213 -- frames until the next shot */
    u8  mSubState;             /* 0x214 -- eye blink step, or the death step */
    u8  mSubTimer;             /* 0x215 -- death wobble frames */
    u8  mCircleTimer;          /* 0x216 -- frames left to keep circling */
    s8  mStarTrackID;          /* 0x217 -- TrackStar slot of the big one's star */
#ifdef __cplusplus
    virtual ~daEykn_c();                          /* slots 16/17 */
    virtual s32 InitResources();             /* slot 0 */
    virtual s32 CleanupResources();          /* slot 3 */
    virtual s32 Behavior();                  /* slot 6 */
    virtual s32 Render();                    /* slot 9 */
    virtual void OnPendingDestroy();         /* slot 12 -- empty body in the ROM */

    /* Coined names: the state table carries no name strings, so every name
       below is descriptive, following daObjCasket_c's SetState, RunState and
       St_*_Init and St_*_Main. */
    int  UpdateCircling();
    int  UpdateEyeAnim();
    void ResetEyeAnim();
    int  StartEyeAnim();
    void LookForPlayer();
    void CheckAttacks();
    void UpdateModelTransform();
    int  St_Die_Main();
    int  St_Die_Init();
    int  St_Attack_Main();
    int  St_Attack_Init();
    int  St_Wait_Main();
    int  St_Wait_Init();
    void RunState();
    void RunStateInit();
    void SetState(int state);

    /* One row of the state table: entered through init, run each frame
       through exec. */
    typedef int (daEykn_c::*StateFunc)();
    struct State {
        StateFunc init;
        StateFunc exec;
    };
#endif
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daEykn_c_size_must_be_0x218[sizeof(struct daEykn_c) == 0x218 ? 1 : -1];
#endif

#endif
