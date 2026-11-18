//cpp
/* The owl actor, daOwl_c (registry profile OWL). It waits unrendered until the
 * player comes near, then hovers around an anchor point, talks to the player
 * (message 0xa2) and, once the talk is over, can pick the player up and carry
 * it.
 * 22 functions, .text 0x02135700..0x021367e8.
 *
 * NAME: _ZTS7daOwl_c is "7daOwl_c" at ov094 0x02136a10; _ZTI at 0x02136a28
 * reads [__si_class_type_info, that string, _ZTI12dEnemyBase_c]. The vtable's
 * address point is 0x02136a58, and slots 16/17 hold D1/D0 below. The tree
 * previously called the class HootTheOwl (coined; vtable address only).
 *
 * STATES: the actor's state is a pointer to a pair of pointers-to-member
 * (enter, main). ov094's __sinit builds five of them from constants in .data.
 * Names are read off what each main handler does:
 *
 *     0x02136b40  dormant   invisible (Render skips it); waits for the player
 *     0x02136b60  hover     bobs at the anchor, wanders, starts the talk
 *     0x02136b50  talk      faces the player until the message ends
 *     0x02136b70  carry     pinned to the rider's matrix
 *     0x02136b30  return    fades out, reappears above the anchor, fades in
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02135700), D0
 * (0x02135748), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 * daOwl_c_classInit (0x02136798..0x021367e8, historical alias
 * HootTheOwl_Spawn) now appends after InitResources, at the end of source
 * order:
 * fBase_c::operator new(size_t) forwards `return new daOwl_c();` to the same
 * _ZN7fBase_cnwEm(1016) allocator the loose factory called by hand, and
 * daOwl_c has no user-declared constructor, so the inherited dEnemyBase_c
 * ctor plus the vtable store plus the four member subobjects in field order
 * (dCcAcPos_c, dBgCh_Actr, dExtAnmModel_c, dExtShadowModel_c) come from the implicit
 * default constructor with zero mangled calls.
 *
 * The fourteen functions whose first parameter was the owl are non-static
 * methods. The cartridge keeps no English spelling. Each Enter/Update pair is
 * one state's enter and main handler. ChangeState still takes the State
 * pointer.
 *
 * The actor-id test in TryPickUpRider goes through the int-returning
 *   IsPlayerActor; a bare `other->actorID != ACTOR_PLAYER` shrinks the method
 *   from ROM 0x110 to object 0x104.
 * Leftover: UpdateDormant stores mPos into `d` and then overwrites all
 *   three fields. Dropping those three stores shrank the method from ROM
 *   0x12c to object 0x114. The stores stay.
 * Leftover: dBgCh_Actr::Unk_0203589c has no member declaration in a shared
 *   header.
 * Leftover: unk_3e8 is scratch that each state uses differently (fade phase,
 *   sound latch, bobbing phase). It stays unnamed.
 * Leftover: the rider matrix pointer at +0xc8, and the model matrix at +0x328
 *   and bone-matrix pointer at +0x320, stay raw offsets from the actor.
 *   Writing them as mAnmModel members hoists the base.
 * Leftover: UpdateHover narrows `player->mStateFlags & 0x800` to u16 before
 *   the test; the ROM masks, then shifts the result through 16 bits.
 *   Kept (#821 r82): the plain mask test -> 0x234 of 0x23c (ands, no shift).
 * Leftover: RandomIntInternal returns int (math/CalcLib.h); UpdateHover and
 *   EnterHover shift it as u32, the ROM's logical shift. Behavior tests the
 *   carry state twice; the else is what the ROM emits.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daOwl_c_AssetId {
    ASSET_OWL_BMD = 0x03cb,
    ASSET_OWL_FLY_1_BCA = 0x03cc,
    ASSET_OWL_FLY_2_BCA = 0x03cd,
    ASSET_OWL_FLY_FREE_BCA = 0x03ce
};

#pragma defer_codegen off

#include "decl_common.h"

/* Spawn offset. 20.12. The middle word is -32.0. */
int data_ov094_02136a1c[3] = {0, 0xfffe0000, 0};
#include "daOwl_c.h"
#include "nitro/fx/fx_trig.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "LevelRng.h"
#include "Sound.h"

/* Resource handles the static initializer constructs. The wrapper spellings
 * are reconstructed; the constructor and destructor addresses, the file IDs
 * and the 8-byte width are the ROM's. */
struct OwlModelFilePtr : SharedFilePtr {
    OwlModelFilePtr(u32 fileID);
    ~OwlModelFilePtr();
};

struct daOwlAnimationFilePtr : SharedFilePtr {
    daOwlAnimationFilePtr(u32 fileID);
    ~daOwlAnimationFilePtr();
};

/* One BSS state: two pointer-to-member constants, copied by the initializer. */
struct OwlState {
    int (daOwl_c::*enter)();
    int (daOwl_c::*main)();

    OwlState(int (daOwl_c::*enterFn)(), int (daOwl_c::*mainFn)())
        : enter(enterFn), main(mainFn) {}
};

typedef char OwlModelFilePtr_size_must_be_8[sizeof(OwlModelFilePtr) == 8 ? 1 : -1];
typedef char daOwlAnimationFilePtr_size_must_be_8[sizeof(daOwlAnimationFilePtr) == 8 ? 1 : -1];
typedef char OwlState_size_must_be_0x10[sizeof(OwlState) == 0x10 ? 1 : -1];

extern OwlModelFilePtr data_ov094_02136ae0;
extern daOwlAnimationFilePtr data_ov094_02136ae8;
extern daOwlAnimationFilePtr data_ov094_02136af0;
extern daOwlAnimationFilePtr data_ov094_02136af8;
extern OwlState data_ov094_02136b30;
extern OwlState data_ov094_02136b40;
extern OwlState data_ov094_02136b50;
extern OwlState data_ov094_02136b60;
extern OwlState data_ov094_02136b70;

enum {
    ACTOR_PLAYER = 0xbf,
    OPACITY_MAX = 0x1f,
    TALK_IDLE = 0,
    TALK_TALKING = 1,
    TALK_DONE = 2
};

#define OWL_STATE(sym)      ((daOwl_c::State *)&(sym))
#define OWL_STATE_DORMANT   OWL_STATE(data_ov094_02136b40)
#define OWL_STATE_HOVER     OWL_STATE(data_ov094_02136b60)
#define OWL_STATE_TALK      OWL_STATE(data_ov094_02136b50)
#define OWL_STATE_CARRY     OWL_STATE(data_ov094_02136b70)
#define OWL_STATE_RETURN    OWL_STATE(data_ov094_02136b30)

#define OWL_TIMER(o)        ((o)->mStateTimer)

/* Whole animation frame of the dExtAnmModel_c, as the ROM extracts it. */
#define OWL_FRAME(o)        ((u16)((o)->mAnmModel.currFrame >> 12))

extern signed char gCurrentSublevel;
extern unsigned char gCurrentStarId;
void func_02012694(unsigned int a, const Vector3 *pos);
short Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
int IsStarCollectedInCurLevel(int);
extern "C" {
int func_ov002_020df7f4(void *c);
void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
}

/* Actor ID 0xbf is PLAYER. This int-returning predicate is what makes the
 * ROM materialize 0/1 before testing it. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == ACTOR_PLAYER;
}

/* The whole body is compiler-emitted: one vptr store, then dExtShadowModel_c
 * (0x370), dExtAnmModel_c (0x30c), dBgCh_Actr (0x150) and dCcAcPos_c (0x110) in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c. D1 is the
 * evidence for the header's four member types; D0 adds dEnemyBase_c's inline
 * operator delete. */
daOwl_c::~daOwl_c()
{
}

/* Hover, once the talk is over: if the player is touching the owl, grab it.
 * The contact owner is the actor id in the hit-box; only the player (0xbf)
 * counts. Attaches the player, zeroes the owl's velocity and enters the carry
 * state; if the attach is refused the rider is dropped again. */
void daOwl_c::TryPickUpRider() {
    Vector3 st;
    Vector3 tmp;
    tmp.x = data_ov094_02136a1c[0];
    tmp.y = data_ov094_02136a1c[1];
    tmp.z = data_ov094_02136a1c[2];
    mdCcAcPos_c.SetPosRelativeToActor(tmp);
    if (mdCcAcPos_c.otherOwner == 0)
        return;
    dActor_c *other = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
    if (other == 0)
        return;
    if (!IsPlayerActor(other))
        return;
    mRider = (daPly_c *)other;
    st.x = 0;
    st.y = 0;
    st.z = 0;
    st.y = 0x1838000;
    if (gCurrentSublevel == 0x16)
        st.y = 0x1194000;
    mSpeed.x = 0;
    mSpeed.y = 0;
    mSpeed.z = 0;
    if (mRider->func_ov002_020df840(this, &st) != 1) {
        mRider = 0;
        return;
    }
    ChangeState(OWL_STATE_CARRY);
}

/* Main of the return state. Phase 0 fades the owl out (with an upward speed);
 * once invisible it jumps to just above the anchor and starts phase 1, which
 * eases it onto the anchor while fading back in, then goes to hover. */
int daOwl_c::UpdateReturn() {
    if (unk_3e8 == 0) {
        if (mOpacity != 0) {
            mOpacity--;
            mPrevAngle.x = 0;
            mSpeed.y = 0x14000;
        } else {
            unk_3e8 = 1;
            mSpeed.x = 0;
            mSpeed.y = 0;
            mSpeed.z = 0;
            mPos.x = mAnchor.x;
            mPos.y = mAnchor.y;
            mPos.z = mAnchor.z;
            mPos.y += 0x64000;
        }
    } else {
        ApproachLinear(mPos, mAnchor, 0x2000);
        if (mOpacity < OPACITY_MAX) {
            mOpacity++;
        } else {
            mOpacity = OPACITY_MAX;
            ChangeState(OWL_STATE_HOVER);
        }
    }
    return 1;
}

/* Enter of the return state: restart the fade and play the animation
 * data_ov094_02136af8[1] at double speed. */
int daOwl_c::EnterReturn() {
    unk_3e8 = 0;
    mStateTimer = 0;
    mAnimSpeed = 0x2000;
    mAnmModel.SetAnim((BCA_File *)data_ov094_02136af8.filePtr, 0, 0x1000, 0);
    return 1;
}

/* Main of the carry state. Once the 30-frame timer has run out it keeps
 * sound 0x18c playing, plays sound 0x139 once per animation loop, levels the pitch, and
 * hands the rider back (to the return state) when the rider lets go, is lost,
 * or the owl hits a wall. */
int daOwl_c::UpdateCarrying() {
    daPly_c *rider;

    if (OWL_TIMER(this) == 1) {
        mAnmModel.SetAnim((BCA_File *)data_ov094_02136ae8.filePtr, 0, 0x1000, 0);
        mAnimSpeed = 0x1000;
    }

    if (OWL_TIMER(this) == 0) {
        mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x18c, mCamSpacePos, 0);
    }

    if (OWL_TIMER(this) != 0) {
        if (unk_3e8 == 0 && OWL_FRAME(this) <= 2) {
            func_02012694(0x139, &mCamSpacePos);
            unk_3e8 = 1;
        } else {
            if (OWL_FRAME(this) > 2)
                unk_3e8 = 0;
        }
    }

    rider = mRider;
    if (rider != 0 && func_ov002_020df7f4(rider) == 1) {
        OWL_TIMER(this) = 0xa;
        mAnmModel.SetAnim((BCA_File *)data_ov094_02136af8.filePtr, 0, 0x1000, 0);
    }

    ApproachAngle(&mPrevAngle.x, 0, 0xa, 0x200, 0x100);

    if (mWithMeshClsn.IsOnWall() != 0 || func_02035638((u8 *)&mWithMeshClsn) != 0) {
        rider = mRider;
        if (rider != 0 && rider->func_ov002_020df7ac() != 0) {
            mWithMeshClsn.Unk_0203589c();
            mRider = 0;
            unk_3e8 = 0;
            mStateTimer = 0;
            ChangeState(OWL_STATE_RETURN);
            return 1;
        }
    }

    rider = mRider;
    /* The second null test is dead, but the ROM keeps it as a real cmp. */
    if (rider == 0 || (rider != 0 && func_ov002_020df7f4(rider) < 0)) {
        mRider = 0;
        unk_3e8 = 0;
        mStateTimer = 0;
        ChangeState(OWL_STATE_RETURN);
    }
    return 1;
}

/* Enter of the carry state: 30-frame timer, animation data_ov094_02136af8[1] at double speed. */
int daOwl_c::EnterCarrying() {
    mStateTimer = 30;
    unk_3e8 = 0;
    mAnmModel.SetAnim((BCA_File *)data_ov094_02136af8.filePtr, 0, 0x1000, 0);
    mAnimSpeed = 0x2000;
    return 1;
}

/* Main of the hover state. Until a conversation has happened it tries to
 * start one with the player (message 0xa2) and, if that takes, enters the talk
 * state. Otherwise it wanders: when it is too far from the anchor it turns
 * back toward it, else it picks a random heading and duration. It bobs on a
 * sine table around the anchor's height and moves along its heading. */
int daOwl_c::UpdateHover() {
    if (mTalkState == TALK_IDLE) {
        if (ClosestPlayer() != 0) {
            daPly_c *player = mTalkTarget;
            if (player != 0) {
                if ((u16)(player->mStateFlags & 0x800) == 0) {
                    if (player->StartTalk(*this, 1) != 0) {
                        if (mTalkTarget->ShowMessage(*this, 0xa2, &mPos, 0, 0) == 1) {
                            func_02012694(0x176, &mCamSpacePos);
                            mTalkState = TALK_TALKING;
                            ChangeState(OWL_STATE_TALK);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    if (Vec3_Dist(&mPos, &mAnchor) > 0x190000) {
        mStateTimer = 50;
        mWanderAngleY = Vec3_HorzAngle(&mPos, &mAnchor);
    } else if (OWL_TIMER(this) == 0) {
        mWanderAngleY = (((u32)RandomIntInternal(&data_0209e650.value) >> 8) << 12);
        mStateTimer = (((u32)RandomIntInternal(&data_0209e650.value) >> 8) & 0x3f) + 50;
    }

    ApproachAngle(&mPrevAngle.y, mWanderAngleY, 0xa, 0x200, 0x100);
    ApproachAngle(&mPrevAngle.x, 0, 0xa, 0x200, 0x100);

    /* The forward step rotated into the speed, and a zero vector nothing
       reads. Without it the function is 0x230 of 0x23c (#821 r49). */
    Vector3 step(0, 0, 0x5000);
    Vector3 unused(0, 0, 0);

    unk_3e8 += 0x200;
    ApproachLinear(mPos.y, mAnchor.y + FX_MUL(FX_SinIdx((u16)(s16)unk_3e8 >> 4), 0x64000), 0x3000);

    Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngle.y);
    MTX_MultVec43(&step, &data_020a0e68, &mSpeed);
    return 1;
}

/* Enter of the hover state: animation data_ov094_02136af0[1] at normal speed, random heading and a
 * 50..113 frame wander timer. */
int daOwl_c::EnterHover() {
    mAnmModel.SetAnim((BCA_File *)data_ov094_02136af0.filePtr, 0, 0x1000, 0);
    mWanderAngleY = (((u32)RandomIntInternal(&data_0209e650.value) >> 8) << 0xc);
    mStateTimer = (((u32)RandomIntInternal(&data_0209e650.value) >> 8) & 0x3f) + 50;
    mAnimSpeed = 0x1000;
    return 1;
}

/* Main of the talk state: turn to face the player (yaw, then pitch toward a
 * point 200 units above its feet). When the player's talk state goes negative
 * the conversation is over; mark it done and return to hover. */
int daOwl_c::UpdateTalk() {
    if (mTalkTarget == 0) {
        ChangeState(OWL_STATE_HOVER);
        return 1;
    }
    /* Kept (#821 r76): `Vector3 v = mTalkTarget->mPos;` is three ldr/str (UpdateTalk 0x100, 9 words
       off); the ROM's ldm/stm needs a POD source. */
    VecFx32 v = *(VecFx32 *)&mTalkTarget->mPos;
    Vector3 w;
    w.x = v.x;
    w.y = v.y;
    w.z = v.z;
    ApproachAngle(&mPrevAngle.y, Vec3_HorzAngle(&mPos, &w), 0xa, 0x200, 0x100);
    Vector3 u;
    v.y += 0xc8000;
    u.x = v.x;
    u.y = v.y;
    u.z = v.z;
    ApproachAngle(&mPrevAngle.x, Vec3_VertAngle(&mPos, &u), 0xa, 0x200, 0x100);
    if (mTalkTarget->GetTalkState() < 0) {
        mTalkState = TALK_DONE;
        ChangeState(OWL_STATE_HOVER);
    }
    return 1;
}

/* Enter of the talk state: stop, and play the animation data_ov094_02136af0[1]. */
int daOwl_c::EnterTalk() {
    mSpeed.x = 0;
    mSpeed.y = 0;
    mSpeed.z = 0;
    mAnmModel.SetAnim((BCA_File *)data_ov094_02136af0.filePtr, 0, 0x1000, 0);
    return 1;
}

/* Main of the dormant state. Watches the nearest player and keeps the owl 400
 * units above that player's ground height. When the player's position is
 * within 40 units of the owl (VEC_Mag < 0x28000) it sets the anchor 800 units
 * from the owl along a fixed yaw, faces it, and enters hover.
 */
int daOwl_c::UpdateDormant() {
    daPly_c *p = ClosestPlayer();
    if (p != 0 && p->mClimbClsn != 0) {
        const Vector3 *playerPos = &p->mPos;
        Vector3 pp;
        Vector3 d;
        pp.x = playerPos->x;
        pp.y = playerPos->y;
        pp.z = playerPos->z;
        mPos.y = p->mGroundY + 0x190000;
        mTalkTarget = p;
        Vec3_Sub(&d, &mPos, &pp);
        if (VEC_Mag(&d) < 0x28000) {
            d.x = mPos.x;
            d.y = mPos.y;
            d.z = mPos.z;
            d.x = 0;
            d.y = 0;
            d.z = 0x320000;
            Matrix4x3_FromRotationY(&data_020a0e68, 0x4000);
            MTX_MultVec43(&d, &data_020a0e68, &mAnchor);
            {
                mAnchor.x += mPos.x;
                mAnchor.y += mPos.y;
                mAnchor.z += mPos.z;
            }
            mPrevAngle.y = Vec3_HorzAngle(&mPos, &mAnchor);
            mAngle.y = mPrevAngle.y;
            ChangeState(OWL_STATE_HOVER);
        }
    }
    return 1;
}

/* Enter of the dormant state: animation data_ov094_02136af0[1]. */
int daOwl_c::EnterDormant() {
    mAnmModel.SetAnim((BCA_File *)data_ov094_02136af0.filePtr, 0, 0x1000, 0);
    return 1;
}

/* Switch state: store it, then run its enter handler if it has one. */
int daOwl_c::ChangeState(State *state) {
    mCurrentState = state;
    State *s = mCurrentState;
    if (s->mEnter == 0)
        return 1;
    return (this->*(s->mEnter))();
}

/* Per-frame model setup: world matrix from position (>> 3) and angles, the
 * model's opacity, its shadow matrix, and the drop shadow. */
void daOwl_c::UpdateModelTransform() {
    Matrix4x3 out;
    Vector3 v;
    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngle.x, mAngle.y, mAngle.z);
    mAnmModel.ApplyOpacity(mOpacity, 1);
    mAnmModel.mat4x3 = data_020a0e68;
    MTX_Concat43(&mAnmModel.data.transforms[1], &mAnmModel.mat4x3, &out);
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x >> 3, (mPos.y - 0x38000) >> 3, mPos.z >> 3);
    mShadowMat = data_020a0e68;
    DropShadowRadHeight(mShadowModel, mShadowMat, 0x64000, 0x320000, 0xf);
}

/* Carry state's placement: take the rider's matrix (pointer at rider + 0xc8),
 * offset and rotate it, and use the result as the owl's own position, yaw and
 * model matrix. */
void daOwl_c::FollowRiderMatrix() {
    daPly_c *m;
    if (mRider == 0)
        return;
    Vector3 v(0, 0, 0);
    m = mRider;
    data_020a0e68 = *m->mCarryMatrix;
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x3000, 0, 0);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, -0x6000, 0x1000, 0x4000);
    v.x = data_020a0e68.t.x;
    v.y = data_020a0e68.t.y;
    v.z = data_020a0e68.t.z;
    mPos.x = v.x << 3;
    mPos.y = v.y << 3;
    mPos.z = v.z << 3;
    mPrevAngle.y = mRider->mAngle.y;
    mPrevAngle.x = 0;
    mAnmModel.mat4x3 = data_020a0e68;
}

int daOwl_c::CleanupResources()
{
    data_ov094_02136ae0.Release();
    data_ov094_02136af8.Release();
    data_ov094_02136ae8.Release();
    data_ov094_02136af0.Release();
    return 1;
}

void daOwl_c::OnPendingDestroy()
{
}

int daOwl_c::Render()
{
    if (mCurrentState == OWL_STATE_DORMANT)
        return 1;
    mAnmModel.Render(0);
    return 1;
}

int daOwl_c::Behavior()
{
    DecIfAbove0_Short(&OWL_TIMER(this));
    {
        State *o = mCurrentState;
        if (o->mMain != 0) {
            (this->*(o->mMain))();
        }
    }
    if (mCurrentState == OWL_STATE_DORMANT)
        return 1;
    mAnmModel.speed = mAnimSpeed;
    mAnmModel.Advance();
    {
        State *m = mCurrentState;
        if ((m == OWL_STATE_TALK || m == OWL_STATE_HOVER ||
             m == OWL_STATE_RETURN) &&
            (unsigned short)(mAnmModel.currFrame >> 0xc) == 0) {
            func_02012694(0x139, &mCamSpacePos);
        }
    }
    /* The ROM tests this state twice; the else is unreachable. */
    if (mCurrentState == OWL_STATE_CARRY) {
        if (mCurrentState == OWL_STATE_CARRY) {
            FollowRiderMatrix();
            mAngle.x = mPrevAngle.x;
            mAngle.y = mPrevAngle.y;
            mAngle.z = mPrevAngle.z;
            UpdateWMClsn(mWithMeshClsn, 0);
        } else {
            UpdateModelTransform();
        }
        return 1;
    }
    {
        int fallSpeed = mSpeed.y + mVertAccel;
        int clamped = mTerminalVelocity;
        int keep = mSpeed.z;
        if (fallSpeed >= clamped)
            clamped = fallSpeed;
        mSpeed.y = clamped;
        mSpeed.z = keep;
    }
    UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    mAngle.x = mPrevAngle.x;
    mAngle.y = mPrevAngle.y;
    mAngle.z = mPrevAngle.z;
    UpdateModelTransform();
    if (mCurrentState == OWL_STATE_HOVER && mTalkState == TALK_DONE) {
        TryPickUpRider();
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

int daOwl_c::InitResources()
{
    Vector3 v0;
    void *f;

    f = dExtSimpleModel_c::LoadFile(data_ov094_02136ae0);
    mAnmModel.SetFile((BMD_File *)f, 1, -1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(data_ov094_02136af8);
    dExtFrameCtrl_c::LoadFile(data_ov094_02136ae8);
    dExtFrameCtrl_c::LoadFile(data_ov094_02136af0);
    v0.x = data_ov094_02136a1c[0];
    v0.y = data_ov094_02136a1c[1];
    v0.z = data_ov094_02136a1c[2];
    mdCcAcPos_c.Init(this, v0, 0x64000, 0x64000, 0x800004, 0);

    mRider = 0;
    mdCcAcPos_c.flags |= 2;
    mTerminalVelocity = -0x1e000;
    mAnimSpeed = 0x1000;
    mOpacity = OPACITY_MAX;
    mWithMeshClsn.Init(this, 0x50000, 0x64000, (Vector3_16 *)0, 0);
    ChangeState(OWL_STATE_DORMANT);

    if (gCurrentSublevel == 7) {
        if (gCurrentStarId == 1 || IsStarCollectedInCurLevel(1) == 0) {
            MarkForDestruction();
            return 0;
        }
    }
    return 1;
}

daOwl_c *daOwl_c_classInit() {
    return new daOwl_c();
}

/* Static-resource ownership, formerly the handwritten __sinit_ov094_021367e8.
 * Definition order is the retail initializer's construction order. The
 * registration nodes and the pointer-to-member descriptors are compiler
 * temporaries. */
OwlModelFilePtr data_ov094_02136ae0(ASSET_OWL_BMD);
daOwlAnimationFilePtr data_ov094_02136af8(ASSET_OWL_FLY_1_BCA);
daOwlAnimationFilePtr data_ov094_02136ae8(ASSET_OWL_FLY_2_BCA);
daOwlAnimationFilePtr data_ov094_02136af0(ASSET_OWL_FLY_FREE_BCA);

OwlState data_ov094_02136b40(&daOwl_c::EnterDormant, &daOwl_c::UpdateDormant);
OwlState data_ov094_02136b50(&daOwl_c::EnterTalk, &daOwl_c::UpdateTalk);
OwlState data_ov094_02136b60(&daOwl_c::EnterHover, &daOwl_c::UpdateHover);
OwlState data_ov094_02136b70(&daOwl_c::EnterCarrying, &daOwl_c::UpdateCarrying);
OwlState data_ov094_02136b30(&daOwl_c::EnterReturn, &daOwl_c::UpdateReturn);
