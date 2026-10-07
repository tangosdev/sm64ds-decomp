/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member types by hand.
 *
 * class daKpa_c: Bowser (ov060), who spawns his tail daKpaTail_c (actor ID 278). The behaviour lives in src/actors/daKpa_c.cpp.
 *
 * Readability pass: members are named from how that file uses them; the C half
 * below repeats the field list for a C translation unit. State, hold-state,
 * condition-flag, actor-ID, sound and collider-flag numbers are the enums at the
 * top of daKpa_c.cpp. Fix12 values read 0x1000 = 1.0; angles 0x10000 = a full turn.
 *
 * Leftover: unk_420 and unk_422 are written in daKpa_c.cpp and never read, so they
 * keep placeholder names; the dActor_c words at 0xa4 and 0xac that the file
 * touches are unnamed (they are dActor_c's own unk_0a4 / unk_0ac); the pad_ arrays
 * are bytes nothing in that file reads or writes.
 *
 * Five sub-objects, and every one's asserted size closes EXACTLY on the next named
 * field -- five independent confirmations of one layout:
 *
 *     dActor_c                      0x000 + 0x0d0 = 0x0d0   -> pad_0d0
 *     ModelAnim                  0x0d4 + 0x064 = 0x138   -> mTextureSequence
 *     TextureSequence            0x138 + 0x014 = 0x14c   -> mWithMeshClsn
 *     dBgCh_Actr               0x14c + 0x1bc = 0x308   -> mShadowModel
 *     dExtShadowModel_c                0x308 + 0x028 = 0x330   -> padding
 *     dCcAcPos_c  0x360 + 0x040 = 0x3a0   -> mTargetPlayer
 *
 * TWO OF THE GENERATED HEADER'S FIELDS WERE THE ModelAnim'S OWN INSIDES and are
 * gone from this half: `mAnimation` at 0x124 is 0x0d4 + 0x50, the dExtFrameCtrl_c base
 * inside ModelAnim, and `unk_130` at 0x130 is 0x0d4 + 0x5c. Both were declared as
 * siblings of a `u8 mModelAnim` marker whose pad stopped short of the real object.
 * Same shape as Player's two ModelAnims.
 *
 * sizeof is 0x454, which is not inferred from the fields: daKpa_c_classInit asks
 * fBase_c::operator new for 1108 bytes -- and the last declared field, mSoundID,
 * happens to end there too.
 *
 * The unk_ entries are placeholders. */
#ifndef DAKPA_C_H
#define DAKPA_C_H
#include "types.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "dBgCh_Actr.h"
#include "dExtShadowModel_c.h"
#include "dCcAcPos_c.h"

#ifdef __cplusplus

#include "dActor_c.h"

struct daKpa_c : dActor_c {
    u8  pad_0d0[0x4];
    ModelAnim mModelAnim;                                   /* 0x0d4 */
    TextureSequence mTextureSequence;                       /* 0x138 */
    dBgCh_Actr mWithMeshClsn;                             /* 0x14c */
    dExtShadowModel_c mShadowModel;                               /* 0x308 */
    /* The shadow's world matrix: a Matrix4x3 written from the scratch matrix at
       0x020a0e68 each frame and handed to dActor_c::DropShadowRadHeight. Left a
       byte array so this header does not have to pull Matrix4x3 in. */
    u8  mShadowMtx[0x30];         /* 0x330 */
    /* The body cylinder. Its dCc_c flags word (+0x18, so 0x378) bit 0 is
       "disabled"; its radius (+0x04, so 0x364) is set to 180 units when the
       defeat knock-back starts; its otherOwner (+0x24, so 0x384) is the uniqueID of
       whatever hit it. */
    dCcAcPos_c mdCcAcPos_c;   /* 0x360 */
    /* A POINTER, not an s32. daKpa_c::Behavior assigns it straight from
       dActor_c::ClosestPlayer() and then re-spelt every read of the slot as
       `*(dActor_c **)((char *)&mTargetPlayer)`; typing it here deletes all three
       of those casts. Declared dActor_c* rather than Player* on purpose --
       Player.h is not includable here, and dActor_c is the base at offset 0, so
       the ClosestPlayer() result converts with no adjustment. */
    dActor_c *mTargetPlayer;      /* 0x3a0 */
    dActor_c *mGrabbedPlayer;     /* 0x3a4 -- the Player holding the tail; null when nobody does */
    /* uniqueID of the KOOPATAIL actor (ID 278) that InitResources spawns. */
    s32 mTailUniqueID;            /* 0x3a8 */
    /* uniqueID of the KOOPA2BG actor (ID 166) found with FindWithActorID. The
       tilt state writes its tilt angles (+0x31e/+0x320/+0x322). */
    u32 mArenaBgUniqueID;         /* 0x3ac */
    s32 mHomePosX;            /* 0x3b0 -- spawn position; Y is the arena floor level */
    s32 mHomePosY;            /* 0x3b4    that the fall and recover tests measure from */
    s32 mHomePosZ;            /* 0x3b8 */
    /* Normal of the floor under him, copied out of the floor result by
       SurfaceInfo::CopyNormalTo (a Vector3 at this address; three words here so
       the header keeps Vector3, which has a destructor, out of the layout). */
    s32 mGroundNormalX;       /* 0x3bc */
    s32 mGroundNormalY;       /* 0x3c0 */
    s32 mGroundNormalZ;       /* 0x3c4 */
    /* Position recorded the last frame he stood on the ground; restored when he
       walks off the floor. */
    s32 mLastGroundPosX;      /* 0x3c8 */
    s32 mLastGroundPosY;      /* 0x3cc */
    s32 mLastGroundPosZ;      /* 0x3d0 */
    /* Two world-space points taken from bone matrices 3 and 6 of his skeleton
       (offsets +0x90 and +0x120 of the bone array), with Y replaced by his own
       Y. Landing dust is spawned at one or the other. */
    s32 mFootPosAX;           /* 0x3d4 */
    s32 mFootPosAY;           /* 0x3d8 */
    s32 mFootPosAZ;           /* 0x3dc */
    s32 mFootPosBX;           /* 0x3e0 */
    s32 mFootPosBY;           /* 0x3e4 */
    s32 mFootPosBZ;           /* 0x3e8 */
    s32 mDistToTarget;            /* 0x3ec -- horizontal distance to mTargetPlayer, 0x7fffffff if none */
    /* Copy of a halfword at +0x69c of the Player holding the tail, refreshed every
       frame while held; its magnitude sets his tilt and, on release, his launch
       speed. */
    s32 mSwingSpeed;          /* 0x3f0 */
    s32 mDistToCenter;        /* 0x3f4 -- horizontal distance from the world origin (the arena centre) */
    s32 mAnimSpeed;            /* 0x3f8 -- written to the ModelAnim's playback speed each frame; 0x1000 = 1.0 */
    u16 mTimer;            /* 0x3fc -- frames in the current state; reset on every state change */
    u16 mStepCounter;         /* 0x3fe -- general per-state counter; its meaning depends on mState */
    u16 mFireTimer;           /* 0x400 -- frames of fire breath so far */
    s16 mSpinSpeed;           /* 0x402 -- angle added to mAngleY each frame in the defeat sequence */
    u8  pad_404[0x2];
    s16 mAngleToTarget;            /* 0x406 */
    s16 mAngleToCenter;       /* 0x408 -- angle from Bowser toward the world origin */
    u8  pad_40a[0x2];
    s32 mState;            /* 0x40c -- see Bowser_State in daKpa_c.cpp */
    s32 mHoldState;           /* 0x410 -- see Bowser_HoldState in daKpa_c.cpp */
    u8  mVariantID;            /* 0x414 -- param1 & 3: which of the three fights; 3 is treated as 0 */
    u8  mPickToggle;          /* 0x415 -- alternates between "choose an action" and "turn to face the target" */
    u8  mChooseJumpOnly;      /* 0x416 -- param1 bit 2; when set, the variant-2 idle pick chooses the jump state instead of calling func_ov060_021150d0 */
    u8  pad_417[0x1];
    u32 mCondFlags;           /* 0x418 -- see Bowser_CondFlag in daKpa_c.cpp */
    u8  mOpacity;            /* 0x41c -- 0..255, walked toward mTargetOpacity by 20 per frame */
    u8  mTargetOpacity;       /* 0x41d */
    s8  mHealth;              /* 0x41e -- hits left; set from a per-variant table (1, 1, 3) */
    u8  pad_41f[0x1];
    u16 unk_420;              /* 0x420 -- zeroed at init, never read in daKpa_c.cpp */
    u8  unk_422;              /* 0x422 -- set to 1 during the hurt hop and the defeat launch and cleared to 0 in the idle state; never read in daKpa_c.cpp */
    u8  mStep;                /* 0x423 -- step within the current state; reset on every state change */
    u8  mTalkStep;            /* 0x424 -- step of the intro conversation (func_ov060_02115518) */
    u8  mHoldStep;            /* 0x425 -- step of the held handler (mHoldState 1) */
    u8  mDropsShadow;            /* 0x426 */
    u8  mBounceOnLand;            /* 0x427 */
    u8  mFireballShots;       /* 0x428 -- shots to fire this time (1..3) */
    u8  mSkipIdleRoll;        /* 0x429 -- set at init; the first variant-0 idle pick that finds mPickToggle set clears it and turns without the one-in-ten draw */
    u8  mVanishChance;        /* 0x42a -- chance in tenths of picking the vanish-and-dash state; starts at 5 */
    u8  mCapActorAlive;            /* 0x42b */
    /* Cutscene camera: look-at target and camera position, written only by the
       intro camera helper func_ov060_02111f08, which feeds them to the Camera. */
    s32 mCamLookAtX;          /* 0x42c */
    s32 mCamLookAtY;          /* 0x430 */
    s32 mCamLookAtZ;          /* 0x434 */
    s32 mCamPosX;             /* 0x438 */
    s32 mCamPosY;             /* 0x43c */
    s32 mCamPosZ;             /* 0x440 */
    u8  mCutsceneStep;            /* 0x444 */
    u8  mCutsceneTimer;       /* 0x445 -- frames spent on the ground in cutscene step 1 */
    u8  mFootfallLatch;            /* 0x446 -- 1 while the animation is inside a footfall frame window */
    u8  pad_447[0x1];
    s32 mParticleHandle;            /* 0x448 */
    s32 mSoundHandle;            /* 0x44c */
    s32 mSoundID;            /* 0x450 */
    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~daKpa_c();                  /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    int InitResources();
    int CleanupResources();
    void OnPendingDestroy();
    int Behavior();
    int Render();

    /* state-table handlers and helpers, in ROM order */
    void func_ov060_02111a28();
    int func_ov060_02111c68();
    void func_ov060_02111cc0(int idx, int animFlags);
    int func_ov060_02111f08();
    void func_ov060_02112350();
    void func_ov060_021123a0(int f);
    void func_ov060_021123c8();
    void func_ov060_021123dc();
    void func_ov060_02112434();
    void func_ov060_021125f0();
    void func_ov060_02112724();
    void func_ov060_021128c0();
    int func_ov060_02112ba8();
    void func_ov060_02112bfc();
    void func_ov060_02112d48(int arg);
    void func_ov060_02112ddc();
    int func_ov060_02112ee0();
    int func_ov060_021130c0();
    void func_ov060_02113260();
    int func_ov060_021132a4();
    int func_ov060_02113404();
    void func_ov060_021134ac();
    void func_ov060_02113564();
    void func_ov060_021135fc();
    void func_ov060_02113710();
    void func_ov060_02113740();
    void func_ov060_02113a94();
    void func_ov060_02113b5c();
    int func_ov060_02113d20();
    void func_ov060_02113d8c();
    void func_ov060_02113fcc();
    int func_ov060_02113ff4(unsigned short arg1, int arg2);
    void func_ov060_021140c0();
    void func_ov060_021142b4();
    void func_ov060_02114300();
    void func_ov060_021143b8();
    void func_ov060_021145a8();
    int func_ov060_021145d4();
    int func_ov060_0211469c();
    void func_ov060_021146d0();
    void func_ov060_02114858();
    void func_ov060_02114b60();
    void func_ov060_02114d08();
    void func_ov060_02114e9c();
    void func_ov060_02114f88();
    void func_ov060_02114ff8();
    void func_ov060_02115018();
    void func_ov060_02115060();
    void func_ov060_021150c4();
    void func_ov060_021150d0();
    void func_ov060_021151d4();
    void func_ov060_02115314();
    void func_ov060_021153f8();
    void func_ov060_021154e8();
    void func_ov060_02115518();
    int func_ov060_021156ec();
    int func_ov060_02115718();
    int func_ov060_02115744();
    void func_ov060_0211577c();
    bool Bowser_IsAnimAtLastFrame();
    void func_ov060_02115a84(char* arg);
    void func_ov060_02115b0c();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Bowser_size_must_be_0x454[sizeof(daKpa_c) == 0x454 ? 1 : -1];
#endif

#else

/* The same object for a C translation unit, which has no base class to inherit
   dActor_c's fields from and so spells the whole layout flat. Every current includer
   is a .cpp; this half is kept so that a future C one gets the right offsets
   rather than a parse error. */
struct daKpa_c {
    u8  pad_000[0x4];
    s32 uniqueID;            /* 0x004 */
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x6];
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x28];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    ModelAnim mModelAnim;                                   /* 0x0d4 */
    TextureSequence mTextureSequence;                       /* 0x138 */
    dBgCh_Actr mWithMeshClsn;                             /* 0x14c */
    dExtShadowModel_c mShadowModel;                               /* 0x308 */
    /* The shadow's world matrix: a Matrix4x3 written from the scratch matrix at
       0x020a0e68 each frame and handed to dActor_c::DropShadowRadHeight. Left a
       byte array so this header does not have to pull Matrix4x3 in. */
    u8  mShadowMtx[0x30];         /* 0x330 */
    /* The body cylinder. Its dCc_c flags word (+0x18, so 0x378) bit 0 is
       "disabled"; its radius (+0x04, so 0x364) is set to 180 units when the
       defeat knock-back starts; its otherOwner (+0x24, so 0x384) is the uniqueID of
       whatever hit it. */
    dCcAcPos_c mdCcAcPos_c;   /* 0x360 */
    /* The C++ half types this dActor_c*; C translation units have no dActor_c
       declaration in scope here, so it is spelt void* -- same width, same slot. */
    void *mTargetPlayer;            /* 0x3a0 */
    void *mGrabbedPlayer;         /* 0x3a4 -- the Player holding the tail; null when nobody does */
    /* uniqueID of the KOOPATAIL actor (ID 278) that InitResources spawns. */
    s32 mTailUniqueID;            /* 0x3a8 */
    /* uniqueID of the KOOPA2BG actor (ID 166) found with FindWithActorID. The
       tilt state writes its tilt angles (+0x31e/+0x320/+0x322). */
    u32 mArenaBgUniqueID;         /* 0x3ac */
    s32 mHomePosX;            /* 0x3b0 -- spawn position; Y is the arena floor level */
    s32 mHomePosY;            /* 0x3b4    that the fall and recover tests measure from */
    s32 mHomePosZ;            /* 0x3b8 */
    /* Normal of the floor under him, copied out of the floor result by
       SurfaceInfo::CopyNormalTo (a Vector3 at this address; three words here so
       the header keeps Vector3, which has a destructor, out of the layout). */
    s32 mGroundNormalX;       /* 0x3bc */
    s32 mGroundNormalY;       /* 0x3c0 */
    s32 mGroundNormalZ;       /* 0x3c4 */
    /* Position recorded the last frame he stood on the ground; restored when he
       walks off the floor. */
    s32 mLastGroundPosX;      /* 0x3c8 */
    s32 mLastGroundPosY;      /* 0x3cc */
    s32 mLastGroundPosZ;      /* 0x3d0 */
    /* Two world-space points taken from bone matrices 3 and 6 of his skeleton
       (offsets +0x90 and +0x120 of the bone array), with Y replaced by his own
       Y. Landing dust is spawned at one or the other. */
    s32 mFootPosAX;           /* 0x3d4 */
    s32 mFootPosAY;           /* 0x3d8 */
    s32 mFootPosAZ;           /* 0x3dc */
    s32 mFootPosBX;           /* 0x3e0 */
    s32 mFootPosBY;           /* 0x3e4 */
    s32 mFootPosBZ;           /* 0x3e8 */
    s32 mDistToTarget;            /* 0x3ec -- horizontal distance to mTargetPlayer, 0x7fffffff if none */
    /* Copy of a halfword at +0x69c of the Player holding the tail, refreshed every
       frame while held; its magnitude sets his tilt and, on release, his launch
       speed. */
    s32 mSwingSpeed;          /* 0x3f0 */
    s32 mDistToCenter;        /* 0x3f4 -- horizontal distance from the world origin (the arena centre) */
    s32 mAnimSpeed;            /* 0x3f8 -- written to the ModelAnim's playback speed each frame; 0x1000 = 1.0 */
    u16 mTimer;            /* 0x3fc -- frames in the current state; reset on every state change */
    u16 mStepCounter;         /* 0x3fe -- general per-state counter; its meaning depends on mState */
    u16 mFireTimer;           /* 0x400 -- frames of fire breath so far */
    s16 mSpinSpeed;           /* 0x402 -- angle added to mAngleY each frame in the defeat sequence */
    u8  pad_404[0x2];
    s16 mAngleToTarget;            /* 0x406 */
    s16 mAngleToCenter;       /* 0x408 -- angle from Bowser toward the world origin */
    u8  pad_40a[0x2];
    s32 mState;            /* 0x40c -- see Bowser_State in daKpa_c.cpp */
    s32 mHoldState;           /* 0x410 -- see Bowser_HoldState in daKpa_c.cpp */
    u8  mVariantID;            /* 0x414 -- param1 & 3: which of the three fights; 3 is treated as 0 */
    u8  mPickToggle;          /* 0x415 -- alternates between "choose an action" and "turn to face the target" */
    u8  mChooseJumpOnly;      /* 0x416 -- param1 bit 2; when set, the variant-2 idle pick chooses the jump state instead of calling func_ov060_021150d0 */
    u8  pad_417[0x1];
    u32 mCondFlags;           /* 0x418 -- see Bowser_CondFlag in daKpa_c.cpp */
    u8  mOpacity;            /* 0x41c -- 0..255, walked toward mTargetOpacity by 20 per frame */
    u8  mTargetOpacity;       /* 0x41d */
    s8  mHealth;              /* 0x41e -- hits left; set from a per-variant table (1, 1, 3) */
    u8  pad_41f[0x1];
    u16 unk_420;              /* 0x420 -- zeroed at init, never read in daKpa_c.cpp */
    u8  unk_422;              /* 0x422 -- set to 1 during the hurt hop and the defeat launch and cleared to 0 in the idle state; never read in daKpa_c.cpp */
    u8  mStep;                /* 0x423 -- step within the current state; reset on every state change */
    u8  mTalkStep;            /* 0x424 -- step of the intro conversation (func_ov060_02115518) */
    u8  mHoldStep;            /* 0x425 -- step of the held handler (mHoldState 1) */
    u8  mDropsShadow;            /* 0x426 */
    u8  mBounceOnLand;            /* 0x427 */
    u8  mFireballShots;       /* 0x428 -- shots to fire this time (1..3) */
    u8  mSkipIdleRoll;        /* 0x429 -- set at init; the first variant-0 idle pick that finds mPickToggle set clears it and turns without the one-in-ten draw */
    u8  mVanishChance;        /* 0x42a -- chance in tenths of picking the vanish-and-dash state; starts at 5 */
    u8  mCapActorAlive;            /* 0x42b */
    /* Cutscene camera: look-at target and camera position, written only by the
       intro camera helper func_ov060_02111f08, which feeds them to the Camera. */
    s32 mCamLookAtX;          /* 0x42c */
    s32 mCamLookAtY;          /* 0x430 */
    s32 mCamLookAtZ;          /* 0x434 */
    s32 mCamPosX;             /* 0x438 */
    s32 mCamPosY;             /* 0x43c */
    s32 mCamPosZ;             /* 0x440 */
    u8  mCutsceneStep;            /* 0x444 */
    u8  mCutsceneTimer;       /* 0x445 -- frames spent on the ground in cutscene step 1 */
    u8  mFootfallLatch;            /* 0x446 -- 1 while the animation is inside a footfall frame window */
    u8  pad_447[0x1];
    s32 mParticleHandle;            /* 0x448 */
    s32 mSoundHandle;            /* 0x44c */
    s32 mSoundID;            /* 0x450 */
};

#endif /* __cplusplus */

#endif
