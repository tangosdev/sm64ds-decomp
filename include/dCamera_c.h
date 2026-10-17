/* dCamera_c's evidenced object layout and real dView_c inheritance.
 * Offsets and widths are observed from matched functions; gaps remain explicit.
 * Field names are descriptive and do not affect code generation. */
#ifndef DCAMERA_C_H
#define DCAMERA_C_H
#include "dView_c.h"

struct dActor_c;

struct dCamera_c : dView_c {
    /* Nested, and only ever needed as a pointer -- dCamera_c::ChangeState is
       mangled `PNS_5StateE`, which IS the nesting, so the declaration is
       evidence rather than convention. Layout not recovered; nothing here
       dereferences one.

       The mangling is the evidence for the nesting. Layout is unnecessary:
       every use is through a pointer. */
    struct State;

    /* dView_c occupies 0x00..0x7f; its Matrix4x3 is at 0x50. */
    /* The camera proper: where it looks and where it is. Both are twelve-byte
       Vector3s -- dCamera_c::SetLookAt and dCamera_c::SetPos each write exactly three
       words, at 0x80 and 0x8c. */
    Vector3 lookAt;            /* 0x080 */
    Vector3 pos;            /* 0x08c */
    /* func_020089f8 copies mTargetPlayer's position here every frame and
       stores lookAt minus it in mLookAtOffset; func_0200bec4 rebuilds lookAt
       as mTargetPos plus mLookAtOffset. */
    Vector3 mTargetPos;    /* 0x098 */
    Vector3 mLookAtOffset; /* 0x0a4 */
    /* The backup pair. SaveCameraStateBeforeTalk copies 0x80->0xb0 and
       0x8c->0xbc as six 4-byte words and then sets flag 0x4000 to mark the
       save taken, so these mirror the two above field for field.
       That copy is also what fixes the WIDTH: 0x094 and 0x0c4 were declared
       u8 and are read and written as full words at both ends, so both were
       too narrow -- they are the z of their respective vectors. */
    Vector3 savedLookAt;            /* 0x0b0 */
    Vector3 savedPos;            /* 0x0bc */
    /* Last frame's pair: Behavior copies lookAt and pos here before it moves
       them, and Render keeps these instead when both have moved by less than
       0x1000. */
    Vector3 mPrevLookAt; /* 0x0c8 */
    Vector3 mPrevPos;    /* 0x0d4 */
    /* The wall func_02009e70's line from above mTargetPos to pos last hit:
       the hit point, and the surface normal CopyNormalTo returns.
       func_02008cb4 ignores a later hit whose normal faces the same way as
       mWallNormal or which lies within 0x80000 of mWallPos. */
    Vector3 mWallPos;    /* 0x0e0 */
    Vector3 mWallNormal; /* 0x0ec */
    /* Render passes these three as G3i::PerspectiveW_'s aspect, near and far
       arguments; func_0200cf40 seeds them with 0x1555, 0x1000 and 0x1388000. */
    s32 mAspect; /* 0x0f8 */
    s32 mNear;   /* 0x0fc */
    s32 mFar;    /* 0x100 */
    u8 pad_104[0x8];
    /* Render packs these four bytes into the VIEWPORT register (0x04000580). */
    u8 mViewport[4];              /* 0x10c */
    s32 mTargetPlayer;            /* 0x110 */
    /* King Bob-omb, King Whomp, Chief Chilly, Goomboss, Bowser and the
       mirror-room Luigi store themselves here every frame; Behavior clears it
       at the end of each frame. In a type-7 camera area func_0200c394 aims
       mLookTargetPos midway between it and mTargetPos. */
    dActor_c *mFramedActor; /* 0x114 */
    /* The Chain Chomp's release steps and the player's level-entry steps
       store themselves here. While it is set, func_02009e70 aims
       mLookTargetPos at it and drops mFramedActor; Behavior clears it and
       sets mFlags bit 4. */
    dActor_c *mFocusActor; /* 0x118 */
    /* LookAtExit points this at the actor's position and copies that
       position into mLookTargetPos. */
    s32 *mLookTarget;       /* 0x11c */
    Vector3 mLookTargetPos; /* 0x120 */
    /* The camera distance used while framing mLookTargetPos: half the
       player-to-mFramedActor distance (func_0200c394, clamped), or 0xdfe60
       for mFocusActor. func_02009e70 eases mFocusBlend toward 0x1000 while
       framing and toward 0 otherwise, and blends the normal near distance
       toward mFocusDist by it. */
    s32 mFocusDist;  /* 0x12c */
    s32 mFocusBlend; /* 0x130 */
    /* Vertical shake: Render adds it to both lookAt.y and pos.y; Behavior
       eases it toward zero and flips its sign every frame. */
    s32 mShakeY; /* 0x134 */
    /* Current state. ChangeState compares its argument against this, and only
       swaps it -- and clears mTurningToTarget -- when they differ. */
    struct State* mState;            /* 0x138 */
    s32 mState_13c;            /* 0x13c */
    u8 pad_140[0x4];
    /* The view object func_0200c66c last chose (GetViewObj's record).
       func_0200bb28 re-reads the base angle only when the object differs. */
    void *mViewObj; /* 0x144 */
    /* Owned; CleanupResources deletes it. */
    void* mFixedViewPos;            /* 0x148 */
    /* The paths func_02009414 walks with func_02008b4c: lookAt along
       mLookAtPath, pos along mPosPath. The func_020082a0 script op sets
       both. */
    void *mLookAtPath; /* 0x14c */
    void *mPosPath;    /* 0x150 */
    /* Flag word. Bit 0 is under-water (dCamera_c::IsUnderwater returns it
       masked, not normalised); 0x10 vetoes a state change; 0x4000 records
       that the pre-talk save has been taken. */
    u32 mFlags;            /* 0x154 */
    /* The handle Sound_PlayIfNotActive returns for the turning sound
       (func_020095e4). */
    s32 mTurnSound; /* 0x158 */
    /* Behavior passes each back to Particle::System::NewWeather and keeps the
       handle it returns; the second is used only with weather 0x86. */
    u32 mWeather;  /* 0x15c */
    u32 mWeather2; /* 0x160 */
    /* func_0200bb28 latches the 0x100/0x200 turn buttons here; the latched
       bit picks which way mBaseAngleOffset steps. */
    u32 mTurnButtons; /* 0x164 */
    /* Behavior eases it toward 0x1000 while flag 0x2000 is set and toward 0
       otherwise; Render blends the field of view toward 0xaaa by it. */
    s32 mFovBlend; /* 0x168 */
    /* Progress within the current path segment (func_02008b4c);
       func_020094a8 clears it with mPathIndex. */
    s32 mPathProgress; /* 0x16c */
    /* Set (func_02008008) and eased (func_02007fcc) by script ops. The
       states read it differently: func_020080b0 as a height, func_020071e0
       as a speed. */
    s32 mScriptParam; /* 0x170 */
    /* func_02007414 zeroes it; func_020072c4 ramps it up to 0xf000 and moves
       savedLookAt and savedPos by it. */
    s32 mRampSpeed; /* 0x174 */
    /* Render feeds mAngleZ to Matrix4x3_FromRotationZ for the view roll, and
       looks mFovAngle up in the sine table for PerspectiveW_'s sin/cos. */
    s16 mAngleZ;   /* 0x178 */
    s16 mFovAngle; /* 0x17a */
    /* The camera's horizontal facing angle. Width is observed, not assumed:
     * every reader in the tree loads it with a 16-bit access --
     * daMugenBGM_c::Behavior and daObjPowerUpItem_c::BeginOpening read it plain,
     * WingFeather::Behavior and func_ov102_021492d4 add 0x4000 to turn it
     * into a billboard facing, and daTree_c::Render feeds it straight to
     * Matrix4x3_FromRotationY. Behavior and func_0200cbe0 write it from
     * Vec3_HorzAngle(lookAt, pos), and mAngleX beside it from
     * Vec3_VertAngle. */
    s16 mAngleY;            /* 0x17c */
    s16 mAngleX;            /* 0x17e */
    /* Yaw as base plus offset. func_0200bb28 eases mEasedAngleY toward
       mBaseAngleY + mBaseAngleOffset and copies it to mAngleY. mBaseAngleY
       comes from a type-2 view object's angle + 0x8000 (also func_0200cf40),
       and the offset steps by 0x2000. func_02005060 seeds the offset with the
       yaw behind the player minus mAngleY; func_02005000 eases it to 0. */
    s16 mEasedAngleY;     /* 0x180 */
    s16 mBaseAngleY;      /* 0x182 */
    s16 mBaseAngleOffset; /* 0x184 */
    /* The yaw func_020091f8 and func_02009e70 turn mAngleY toward while
       mTurningToTarget is set. */
    s16 mTargetAngleY; /* 0x186 */
    u8 pad_188[0x4];
    /* Shake amplitudes. Render adds mRollShake times the sine of the phase at
       0x18a to the roll, and mFovShake times the cosine of the phase at 0x192
       to the field of view; Behavior decays both. */
    s16 mRollShake; /* 0x18c */
    s16 mFovShake;  /* 0x18e */
    u8 pad_190[0x6];
    /* Countdown. func_020098b8 arms it (0x1c) and func_0200987c switches to
       state 0x14 when it runs out; func_0200bb28 reuses it as the repeat
       delay (0x14) between turn-button steps. */
    u16 mStateTimer; /* 0x196 */
    /* The path segment func_02008b4c is in; func_020094a8 clears it. */
    s16 mPathIndex; /* 0x198 */
    /* The yaw func_020070e8 orbits pos toward: func_02007758 always passes
       it, and func_02007a8c passes it when the script's yaw is -1. Seeded from
       mFixedViewPos or per level (func_02007d58) and from mAngleY
       (func_0200d4b0). */
    s16 mOrbitTargetYaw; /* 0x19a */
    /* func_02008b08 eases it toward the active player's camera-input entry;
       func_02008abc and func_02008a70 scale the input axes by it. */
    s16 mInputScale; /* 0x19c */
    /* The yaw behind the player (its facing + 0x8000), taken when turn
       button 0x4000 is pressed. mRecentering stays nonzero while
       ApproachAngle is still swinging the camera to it (func_02009e70,
       func_0200bb28). */
    s16 mBehindAngleY; /* 0x19e */
    u16 mRecentering;  /* 0x1a0 */
    /* Accumulated by the func_020079ac script op and added to the pitch and
       yaw it orbits lookAt by; func_02007d58 zeroes both. */
    s16 mPitchOffset; /* 0x1a2 */
    s16 mYawOffset;   /* 0x1a4 */
    /* Set when a new mTargetAngleY is chosen (func_02009d30 on entry,
       func_02009e70 on a view-object yaw or a wall). While set, the camera
       turns toward it; it clears on arrival. ChangeState, turn input and
       func_02008cb4's push-in also clear it. */
    u8 mTurningToTarget; /* 0x1a6 */
    /* The path node func_0200b0fc is past: cleared while mFlags bit 4 is set,
       then stepped as lookAt nears the next or previous node. */
    u8 mPathNode; /* 0x1a7 */
    dCamera_c();
    virtual ~dCamera_c();

    /* fBase_c virtual overrides, in their inherited slots. */
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();

    /* Non-virtual camera operations. */
    int ChangeState(State * state);
    void GoBehindPlayer(u32 playerID);
    int IsUnderwater() const;
    void SaveCameraStateBeforeTalk();
    void LookAtExit(dActor_c &actor);
    void SetFlag_3();
    void SetLookAt(const Vector3 & lookAt_);
    void SetPos(const Vector3 & pos_);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Camera_size_must_be_0x1a8[sizeof(struct dCamera_c) == 0x1a8 ? 1 : -1];
#endif

#endif
