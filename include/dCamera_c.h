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
    u8  pad_098[0x18];
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
    u8 pad_0e0[0x18];
    /* Render passes these three as G3i::PerspectiveW_'s aspect, near and far
       arguments; func_0200cf40 seeds them with 0x1555, 0x1000 and 0x1388000. */
    s32 mAspect; /* 0x0f8 */
    s32 mNear;   /* 0x0fc */
    s32 mFar;    /* 0x100 */
    u8 pad_104[0x8];
    /* Render packs these four bytes into the VIEWPORT register (0x04000580). */
    u8 mViewport[4];              /* 0x10c */
    s32 mTargetPlayer;            /* 0x110 */
    /* d_a_btn.cpp stores an actor pointer through pad_114 by name, so that
       word stays a pad. */
    u8 pad_114[0x8];
    /* LookAtExit points this at the actor's position and copies that
       position into mLookTargetPos. */
    s32 *mLookTarget;       /* 0x11c */
    Vector3 mLookTargetPos; /* 0x120 */
    u8 pad_12c[0x8];
    /* Vertical shake: Render adds it to both lookAt.y and pos.y; Behavior
       eases it toward zero and flips its sign every frame. */
    s32 mShakeY; /* 0x134 */
    /* Current state. ChangeState compares its argument against this, and only
       swaps it -- and clears unk_1a6 -- when they differ. */
    struct State* mState;            /* 0x138 */
    s32 mState_13c;            /* 0x13c */
    u8  pad_140[0x8];
    /* Owned; CleanupResources deletes it. */
    void* mFixedViewPos;            /* 0x148 */
    u8  pad_14c[0x8];
    /* Flag word. Bit 0 is under-water (dCamera_c::IsUnderwater returns it
       masked, not normalised); 0x10 vetoes a state change; 0x4000 records
       that the pre-talk save has been taken. */
    u32 mFlags;            /* 0x154 */
    u8 pad_158[0x10];
    /* Behavior eases it toward 0x1000 while flag 0x2000 is set and toward 0
       otherwise; Render blends the field of view toward 0xaaa by it. */
    s32 mFovBlend; /* 0x168 */
    u8 pad_16c[0xc];
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
    u8 pad_180[0xc];
    /* Shake amplitudes. Render adds mRollShake times the sine of the phase at
       0x18a to the roll, and mFovShake times the cosine of the phase at 0x192
       to the field of view; Behavior decays both. */
    s16 mRollShake; /* 0x18c */
    s16 mFovShake;  /* 0x18e */
    u8 pad_190[0x16];
    u8  unk_1a6;            /* 0x1a6 */
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
