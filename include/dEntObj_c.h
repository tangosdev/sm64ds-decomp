/* The multiplayer entry scene object. Its two ROM destructor variants reveal
 * the complete ownership chain: dBase_c, Particle::SysTracker, Model,
 * ModelAnim, and four 0x158-byte player records. The record constructor and
 * destructor were previously anonymous address symbols; their four member
 * constructor/destructor calls prove the typed prefix below, while the array
 * stride proves its trailing extent.
 *
 * The class name is the ROM's own: _ZTS9dEntObj_c at ov075:0x0211c648, with
 * _ZTI9dEntObj_c at 0x0211c66c and the vtable _ZTV9dEntObj_c at 0x0211c6a0.
 * UnknownVsPlayer has no vtable and no RTTI, so the cartridge gives it no
 * name and the project's name stays. */
#ifndef DENTOBJ_C_H
#define DENTOBJ_C_H
#include "types.h"

#ifdef __cplusplus

#include "dBase_c.h"
#include "Particle__SysTracker.h"
#include "Model.h"
#include "ModelAnim.h"
#include "BlendModelAnim.h"
#include "TextureSequence.h"
#include "dExtShadowModel_c.h"

/* One figure on the entry stage. mMoveState and mAnimState index the two
 * pointer-to-member state tables in src/actors/dEntObj_c.cpp; the fields
 * after them are named from what those states read and write. */
struct UnknownVsPlayer {
    BlendModelAnim mModel;             /* 0x000 */
    ModelAnim mAnimation;              /* 0x070 */
    TextureSequence mTextureSequence;  /* 0x0d4 */
    dExtShadowModel_c mShadow;               /* 0x0e8 */
    s32 mMoveState;                    /* 0x110 */
    s32 mAnimState;                    /* 0x114 */
    Vector3 mPosition;                 /* 0x118 */
    Vector3 mTargetPos;                /* 0x124 - walk target */
    Vector3 mExitPos;                  /* 0x130 - jump-off target */
    s32 mSpeed;                        /* 0x13c */
    s32 mMaxSpeed;                     /* 0x140 */
    s32 mVertSpeed;                    /* 0x144 */
    s32 mMaterialColor;                /* 0x148 - written to +0x20 of every material each
                                          frame; materials[0]'s value + 2 * mPlayerNo */
    u32 mParticleHandle;               /* 0x14c */
    s16 mAngleY;                       /* 0x150 */
    u8 mPlayerNo;                      /* 0x152 - slot in dEntObj_c::mPlayers */
    u8 mInAir;                         /* 0x153 - also a once-flag in movement state 8 */
    u8 mFellOff;                       /* 0x154 */
    u8 unk_155;                        /* 0x155 - set the frame it falls off */
    u8 mWaitTimer;                     /* 0x156 */
    u8 pad_157;

    UnknownVsPlayer();
    ~UnknownVsPlayer();

    /* The mMoveState handlers (data_ov075_0211d56c's indices). */
    void func_ov075_021147d4();        /* 1 */
    void func_ov075_0211478c();        /* 2 */
    void func_ov075_0211473c();        /* 3 */
    void func_ov075_02114560();        /* 5 */
    void func_ov075_021143e4();        /* 7 */
    void func_ov075_02114390();        /* 8 */
    void func_ov075_02114300();        /* 9 */
    /* The mAnimState handlers (data_ov075_0211d53c's indices). */
    int func_ov075_0211427c();         /* 1 */
    int func_ov075_02114218();         /* 2 */
    int func_ov075_021141b8();         /* 3 */
    void func_ov075_021140e4();        /* 4 */
    void func_ov075_02114010();        /* 5 */
    /* The empty handler both tables' unused indices point at. */
    void func_ov075_02114890();
    void func_ov075_021142fc();

    /* Send the figure toward its exit slot at x = exitX. */
    void func_ov075_02114904(int exitX);
    /* The selection one-shot on this figure; returns 1 when armed. */
    int func_ov075_02114988();
    /* Walk the figure toward x = targetX. */
    void func_ov075_021149d0(int targetX);
    int func_ov075_02114a58();         /* movement state == 0 (idle) */
    int func_ov075_02114a6c();         /* go idle in the standing animation */
    /* dCamera_c follow for the focused figure; nonzero when the view needs
       rebuilding. */
    int func_ov075_02114ac4(Vector3 *at, Vector3 *pos);
    void func_ov075_02114894();
    int func_ov075_021148f0();
    void func_ov075_02114b60();
    void func_ov075_02114be4();
    /* One figure's frame: movement state, model matrix, animation state,
       both animators, the shadow. */
    void func_ov075_02114cd8();
    /* Set up the figure for player slot playerNo at x. */
    int func_ov075_02114ddc(unsigned char kind, unsigned char playerNo, int x);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char UnknownVsPlayer_size_must_be_0x158[
    sizeof(UnknownVsPlayer) == 0x158 ? 1 : -1];
#endif

struct dEntObj_c : dBase_c {
    Particle::SysTracker mParticles;   /* 0x050 */
    Model mModel;                      /* 0x86c */
    ModelAnim mModelAnim;              /* 0x8bc */
    UnknownVsPlayer mPlayers[4];       /* 0x920 */
    u8 unk_e80;                        /* 0xe80 */
    u8 pad_e81[0xa7];
    s32 mCamPosX;                      /* 0xf28 */
    s32 mCamPosY;                      /* 0xf2c */
    s32 mCamPosZ;                      /* 0xf30 */
    s32 mCamTargetX;                   /* 0xf34 */
    s32 mCamTargetY;                   /* 0xf38 */
    s32 mCamTargetZ;                   /* 0xf3c */
    u8 mAnimActive;                    /* 0xf40 */
    u8 mState;                         /* 0xf41 */
    u8 mFocusedPlayer;                 /* 0xf42 */
    u8 mPlayerCount;                   /* 0xf43 */
    u8 mSuspended;                     /* 0xf44 */

    virtual ~dEntObj_c();
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    /* Send every present figure toward its exit and pick the one the camera
       follows. */
    void func_ov075_02114fa8();
    /* Stop the selection animation and return the chosen figure to idle. */
    void func_ov075_0211505c();
    /* Once every figure is idle, play the selection one-shot on player
       playerNo's figure; returns 0 while any figure still moves. */
    int func_ov075_02115098(int playerNo);
    /* Walk every figure to its slot for the current player count. */
    void func_ov075_02115134();
    /* Put the selection model over player playerNo's figure. */
    void func_ov075_021151b4(int playerNo);
    /* The exit-slot x for figure playerNo. */
    int func_ov075_0211524c(int playerNo);
    /* The slot x for figure playerNo under the current player count. */
    int func_ov075_02115290(int playerNo);
    /* Copy the camera target/position fields into the shared block. */
    void func_ov075_021152d4();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dEntObj_c_size_must_be_0xf48[sizeof(dEntObj_c) == 0xf48 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif
