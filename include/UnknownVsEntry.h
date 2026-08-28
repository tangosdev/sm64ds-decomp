/* The multiplayer entry scene object. Its two ROM destructor variants reveal
 * the complete ownership chain: dBase_c, Particle::SysTracker, Model,
 * ModelAnim, and four 0x158-byte player records. The record constructor and
 * destructor were previously anonymous address symbols; their four member
 * constructor/destructor calls prove the typed prefix below, while the array
 * stride proves its trailing extent. */
#ifndef UNKNOWNVSENTRY_H
#define UNKNOWNVSENTRY_H
#include "types.h"

#ifdef __cplusplus

#include "dBase_c.h"
#include "Particle__SysTracker.h"
#include "Model.h"
#include "ModelAnim.h"
#include "BlendModelAnim.h"
#include "TextureSequence.h"
#include "ShadowModel.h"

struct UnknownVsPlayer {
    BlendModelAnim mModel;             /* 0x000 */
    ModelAnim mAnimation;              /* 0x070 */
    TextureSequence mTextureSequence;  /* 0x0d4 */
    ShadowModel mShadow;               /* 0x0e8 */
    u8 pad_110[0x8];
    Vector3 mPosition;                 /* 0x118 */
    u8 pad_124[0x31];
    u8 unk_155;                        /* 0x155 */
    u8 pad_156[0x2];

    UnknownVsPlayer();
    ~UnknownVsPlayer();
};

typedef char UnknownVsPlayer_size_must_be_0x158[
    sizeof(UnknownVsPlayer) == 0x158 ? 1 : -1];

struct UnknownVsEntry : dBase_c {
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

    virtual ~UnknownVsEntry();
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#else

/* Flat compatibility view for C consumers. */
struct UnknownVsEntry {
    u8 pad_000[0x8];
    s32 mParam;                        /* 0x008 */
    u8  pad_00c[0x44];
    u8  mParticle;                     /* 0x050 */
    u8  pad_051[0x81b];
    u8  pad_86c[0x614];
    u8  unk_e80;                     /* 0xe80 */
    u8  pad_e81[0xa7];
    s32 mCamPosX;                     /* 0xf28 */
    s32 mCamPosY;                     /* 0xf2c */
    s32 mCamPosZ;                     /* 0xf30 */
    s32 mCamTargetX;                  /* 0xf34 */
    s32 mCamTargetY;                  /* 0xf38 */
    s32 mCamTargetZ;                  /* 0xf3c */
    u8  mAnimActive;                  /* 0xf40 */
    u8  mState;                       /* 0xf41 */
    u8  mFocusedPlayer;               /* 0xf42 */
    u8  mPlayerCount;                 /* 0xf43 */
    u8  mSuspended;                   /* 0xf44 */
};

#endif /* __cplusplus */

typedef char UnknownVsEntry_size_must_be_0xf48[sizeof(struct UnknownVsEntry) == 0xf48 ? 1 : -1];

#endif
