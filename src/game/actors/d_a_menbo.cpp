//cpp
/* daMenbo_c -- Skeeter, the water strider on the lake.
 *
 * ov090 0x02130f00..0x0213269c, twenty-five functions, the last being the
 * registry factory daMenbo_c_classInit. daManta_c opens the next run at
 * 0x0213269c. g_profile_MENBO stays out of this file. One out-of-line destructor
 * is the key function: it emits D1 then D0 and anchors the vtable.
 *
 * This TU also emits its own __sinit: five file-handle globals (one model,
 * four anims) are constructed and destructor-registered, then the eight
 * pointer-to-member descriptors at 0x021340e0 are copied into the four
 * state nodes. Entry is the first pair, update the second:
 *   data_ov090_021344e4  idle         02131608 / 02131584   anim 02134498
 *   data_ov090_02134514  patrol       02131a74 / 02131648   anim 02134480
 *   data_ov090_02134504  water rest   02131b94 / 02131ac4   anim 02134490
 *   data_ov090_021344f4  water chase  02131db0 / 02131c48   anim 02134488
 * Idle and patrol hand off to water rest while mOnWater is set (the skeeter
 * is on the water line). Water chase hands back to idle when it clears.
 * Patrol itself speeds up when a player is close; that is not a fifth state.
 *
 * defer_codegen is off, so the bodies are
 * ROM-ascending. No optimization pragma: the file used to compile with
 * opt_strength_reduction off for the sake of func_ov090_02130f94 alone, whose
 * ripple loop carried a hand-reduced `idx += 3` accumulator. Written as the
 * real `for (i = 0; i < 4; ++i)` with the bone index spelled `6 + i * 3`, the
 * loop reproduces with strength reduction on and the other 24 functions never
 * needed it (measured: with the pragma gone and the accumulator kept, that one
 * function is 0x124 against the ROM's 0x120).
 */

#pragma defer_codegen off

#include "common.h"
#include "daMenbo_c.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "dExtFrameCtrl_c.h"
#include "math/Approach.h"
#include "Particle__System.h"
#include "nitro/fx/fx.h"
#include "LevelRng.h"
#include "math/MtxStack.h"

struct BMD_File;
struct BCA_File;

/* 8-byte file handles: filePtr is the loaded file. The model constructs
 * with func_02017acc and destroys with func_02017ab4; the anims use
 * SharedFilePtr::Construct and
 * SharedFilePtr_Destruct_Anim. Manifest aliases carry the spellings. */
struct MenboModelFilePtr : SharedFilePtr {
    MenboModelFilePtr(u32 fileID);
    ~MenboModelFilePtr();
};

struct MenboAnimationFilePtr : SharedFilePtr {
    MenboAnimationFilePtr(u32 fileID);
    ~MenboAnimationFilePtr();
};

extern "C" void Vec3_Lsl(Vector3 *out, const Vector3 *in, int n);
extern "C" void Vec3_Asr(void *dst, void *src, int n);
/* local extern: CalcLib.h clashes with this TU's C Vec3_Asr (void *) and Vec3_Lsl (non-const source) */
extern "C" int Vec3_Dist(const Vector3 *a, const Vector3 *b);
/* local extern: CalcLib.h clashes with this TU's C Vec3_Asr (void *) and Vec3_Lsl (non-const source) */
extern "C" int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
/* local extern: CalcLib.h clashes with this TU's C Vec3_Asr (void *) and Vec3_Lsl (non-const source) */
extern "C" s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern "C" int AngleDiff(int a, int b);
extern "C" int ApproachAngle(s16 *cur, s16 target, int divisor, int band, int maxStep);
extern "C" u16 DecIfAbove0_Short(u16 *p);
extern "C" int RandomIntInternal(int *seed);
void func_02012694(unsigned int id, const Vector3 *pos);

int SurfaceInfo_TestFlag0x20(int *p);
extern "C" int func_02035638(u8 *p);

extern int           data_02092138;
extern unsigned char gGameMode;
extern signed char gCurrentSublevel;
extern int           data_0209f32c;

extern Vector3       data_ov090_0213412c;
extern MenboAnimationFilePtr data_ov090_02134480; /* patrol anim, file 0x3a1 */
extern MenboAnimationFilePtr data_ov090_02134488; /* water-chase anim, file 0x3a0 */
extern MenboAnimationFilePtr data_ov090_02134490; /* water-rest anim, file 0x3a2 */
extern MenboAnimationFilePtr data_ov090_02134498; /* idle anim, file 0x3a3 */
extern MenboModelFilePtr     data_ov090_021344a0; /* model, file 0x39f */
/* Each node is the (entry, update) PMF pair the Behavior installs.
 * e4 idle, f4 water chase, 504 water rest, 514 patrol. */
extern daMenbo_c::StateFn data_ov090_021344e4[2];
extern daMenbo_c::StateFn data_ov090_021344f4[2];
extern daMenbo_c::StateFn data_ov090_02134504[2];
extern daMenbo_c::StateFn data_ov090_02134514[2];

/* dActor_c::mFlags bit 0x08 is written by the framework while the actor is
 * off screen. Actor ID 0xbf is PLAYER. gGameMode is the current game
 * mode. These int-returning predicates are what make the ROM materialize
 * 0/1 before testing it. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

static inline int IsGameMode(int mode) {
    return gGameMode == mode;
}

daMenbo_c::~daMenbo_c()
{
}

/* One ripple under each of four leg bones. Skipped while the actor is off
 * screen. Bone matrices are transforms[6], [9], [12] and [15], multiplied
 * by the model matrix; the ripple sits on the water line. */
void daMenbo_c::func_ov090_02130f94() {
    int zero;
    int sh;
    int i;
    Matrix4x3 *src;
    unsigned int id;

    if (IsOffScreen(this))
        return;

    src = &mAnmModel.mat4x3;
    zero = 0;
    sh = 3;
    id = 0xea;
    for (i = 0; i < 4; ++i) {
        Vector3 v;
        Vector3 r;

        data_020a0e68 = *src;
        MTX_Concat43((const Matrix4x3 *)mAnmModel.data.transforms + (6 + i * 3), &data_020a0e68,
                     &data_020a0e68);
        v.x = data_020a0e68.t.x;
        v.y = data_020a0e68.t.y;
        v.z = data_020a0e68.t.z;
        Vec3_Lsl(&r, &v, sh);
        v.x = r.x;
        v.y = r.y;
        v.z = r.z;
        v.y = mSurfaceY;
        mRippleParticles[i] = Particle::System::New(mRippleParticles[i], id, v.x, v.y, r.z,
                                                    (const Vector3_16f *)(zero),
                                                    (dPa_c::level_c::callback_c *)(zero));
    }
}

/* Contact response, in the order the tests run. */
void daMenbo_c::func_ov090_021310b4() {
    Vector3_16 hv;
    Vector3 sv;
    Vector3 cv;
    Vector3 hurt;
    int flags;
    dActor_c *p;
    sv = data_ov090_0213412c;
    mdCcAcPos_c.SetPosRelativeToActor(sv);

    if (mdCcAcPos_c.otherOwner == 0)
        return;
    if ((p = dActor_c::FindWithID(mdCcAcPos_c.otherOwner)) == 0)
        return;
    /* Measured (#821 r68): a plain `flags = mdCcAcPos_c.hitFlags` (int or u32
       local, either declaration order) swaps r4/r5 through the function. */
    /* Kept (#821 r76): a register-order pin; no block-copy or type lever (r72-r75) applies to this
       scalar pin.
       Kept (#821 r80): getter tried -> a hit-flags accessor, int or u32, is 0x2c4 with r4/r5
       swapped (26 words). */
    flags = (int)((long long)*(int *)&mdCcAcPos_c.hitFlags);

    if (flags & 0x2400) {
        mDeathState = 2;
        func_ov002_020aea30(p, 0);
        return;
    }
    if (flags & 0x40000) {
        mDeathState = 4;
        func_ov002_020aea30(p, 0);
        return;
    }
    if (flags & 0x40) {
        mDeathState = 2;
        func_ov002_020aea30(p, 0);
        return;
    }
    if (JumpedOnByPlayer(mdCcAcPos_c, *(daPly_c *)p)) {
        ((daPly_c *)p)->Bounce(0x28000);
        mDeathState = 1;
        func_ov002_020aea30(p, 0);
        return;
    }
    if (flags & 0x20) {
        mVertAccel = 0;
        mSpeed.x = 0;
        mSpeed.y = 0;
        mSpeed.z = 0;
        mDeathState = 1;
        func_ov002_020aea30(p, 0);
        return;
    }
    if (flags & 0x10) {
        hv.x = 0x1000;
        hv.y = 0;
        hv.z = 0;
        KillByInvincibleChar(hv, *(daPly_c *)p, 0);
        func_02012694(0x1d, &mCamSpacePos);
        return;
    }
    if (flags & 0x4380) {
        mDeathTumble = 3;
        mDeathState = 3;
        func_ov002_020aea30(p, 0);
        return;
    }
    if (!IsPlayerActor(p))
        return;
    if (((daPly_c *)p)->mIsMetal == 1 || ((daPly_c *)p)->IsOnShell() == 1) {
        cv.x = mPos.x;
        cv.y = mPos.y;
        cv.z = mPos.z;
        SpawnCoins(cv, mExtraCoinCount + 1, 0xa000, 0);
        PoofDust();
        KillAndTrackInDeathTable();
        return;
    }
    hurt.x = mPos.x;
    hurt.y = mPos.y;
    hurt.z = mPos.z;
    ((daPly_c *)p)->Hurt(hurt, 2, 0xc000, 1, 0, 1);
}

/* Ground probe. Mode 2 on course 0x12 skips it and only spawns ripples.
 * Otherwise a hit below the published water height (data_0209f32c) means
 * the skeeter is on the water: record that line, mark mOnWater, ripple.
 * A higher floor is the line instead, and either way Y is clamped up to it. */
void daMenbo_c::func_ov090_02131378() {
    if (IsGameMode(2) && gCurrentSublevel == 0x12) {
        this->func_ov090_02130f94();
        return;
    }
    dBgCh_Gnd rg;
    Vector3 v;
    int vz = mPos.z;
    int vy = mPos.y + 0x32000;
    v.x = mPos.x;
    v.y = vy;
    v.z = vz;
    mOnWater = 0;
    if (Vec3_HorzDist(&mPos, &mPrevPos) != 0) {
        rg.SetObjAndPos(v, this);
        if (rg.DetectClsn() != 0)
            mFloorY = rg.clsnY;
    }
    if (mFloorY < data_0209f32c) {
        mSurfaceY = data_0209f32c;
        if (mPos.y <= mSurfaceY) {
            mOnWater = 1;
            this->func_ov090_02130f94();
        }
    } else {
        mSurfaceY = mFloorY;
    }
    if (mPos.y <= mSurfaceY) {
        mPos.y = mSurfaceY;
        mSpeed.y = 0;
    }
}

/* 1 while a player is close and within a quarter turn of the heading.
 * The held target latches; releasing it points the skeeter back at its
 * anchor and sets the patrol speed. */
int daMenbo_c::func_ov090_021314a0() {
    daPly_c *p = this->ClosestNonVanishPlayer();
    if (p != 0 && AngleDiff(this->mPrevAngle.y, this->HorzAngleToCPlayer()) < 0x2000) {
        int *sv = &p->mPos.x;
        int v[3];
        v[0] = sv[0];
        v[1] = sv[1];
        v[2] = sv[2];
        if (Vec3_Dist(&this->mPos, (const Vector3 *)v) < 0x3ac000) {
            int w[3];
            w[0] = v[0];
            w[1] = v[1];
            w[2] = v[2];
            this->mTargetAngleY = Vec3_HorzAngle(&this->mPos, (const Vector3 *)w);
            this->mChaseLatch = 1;
            return 1;
        }
    }
    if (this->mChaseLatch == 1) {
        this->mHorzSpeed = 0x9000;
        this->mChaseLatch = 0;
        this->mTargetAngleY = Vec3_HorzAngle(&this->mPos, &this->mAnchorPos);
    }
    return 0;
}

/* Idle. Speed stays zero. After a few loops of the idle anim, or as soon
 * as a player is in range, start patrolling. On the water, rest instead. */
int daMenbo_c::func_ov090_02131584()
{
    unsigned int v;
    mHorzSpeed = 0;
    v = ((unsigned int)mAnmModel.currFrame << 4) >> 0x10;
    if (v >= 0x3b)
        mAnimLoops += 1;
    if (mAnimLoops > 2 || this->func_ov090_021314a0() == 1)
        this->func_ov090_02131e00(data_ov090_02134514);
    if (mOnWater == 1)
        this->func_ov090_02131e00(data_ov090_02134504);
    return 1;
}

/* Idle entry. */
int daMenbo_c::func_ov090_02131608()
{
    mAnimLoops = 0;
    mAnimSpeed = 0x1000;
    mAnmModel.SetAnim((BCA_File *)data_ov090_02134498.filePtr, 0, 0x1000, 0);
    return 1;
}

/* Patrol. A line probe ahead, the distance back to the anchor, and a wall
 * each turn it around, and each latches so the turn only fires on the edge.
 * A turn reloads the three timers and snaps the position back to last
 * frame's. The heading then eases toward mTargetAngleY. Close to a player, the
 * playback rate and the speed both double for a bit. */
int daMenbo_c::func_ov090_02131648() {
    int dist;
    u32 rnd;
    int selfY;
    Vector3 a, b, in, out;
    int angleSet;
    s16 *p39a;
    int sh;
    s16 ha;
    s32 *p390;

    rnd = (u32)RandomIntInternal(&data_0209e650.value) >> 8;
    dist = Vec3_Dist(&mPos, &mAnchorPos);

    if (((((u32)mAnmModel.currFrame) << 4) >> 0x10 & 0xf) == 0) {
        func_02012694(0xfc, &mCamSpacePos);
    }

    angleSet = 0;
    dBgCh_Lin line;

    a.x = 0; a.y = 0; a.z = 0;
    b.x = 0; b.y = 0; b.z = 0;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    a.x = mPos.x;
    selfY = mPos.y;
    a.y = selfY;
    a.z = mPos.z;
    a.y = selfY + 0x64000;
    in.y = 0x64000;
    in.z = 0x1f4000;

    Matrix4x3_FromRotationY(&data_020a0e68, mAngle.y);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, 0x2000);
    MTX_MultVec43(&in, &data_020a0e68, &out);

    b.x = a.x;
    b.x = a.x + out.x;
    b.y = a.y;
    b.z = a.z;
    b.y = a.y + out.y;
    b.z = a.z + out.z;

    line.SetObjAndLine(a, b, this);

    if (!line.DetectClsn()) {
        if (mProbeLatch == 0) {
            ha = Vec3_HorzAngle(&mPos, &mAnchorPos);
            sh = (rnd & 3) << 0xc;
            mTargetAngleY = ha;
            p39a = &mTargetAngleY;
            angleSet = 1;
            *p39a = *p39a + (0x1800 - sh);
            mPos.x = mPrevPos.x;
            mPos.y = mPrevPos.y;
            mPos.z = mPrevPos.z;
            mProbeLatch = 1;
        }
    } else {
        mProbeLatch = (u8)angleSet;
    }

    if (dist > 0x3c0000) {
        if (mFarLatch == 0 && mProbeLatch == 0) {
            ha = Vec3_HorzAngle(&mPos, &mAnchorPos);
            mTargetAngleY = ha;
            angleSet = 1;
            mPos.x = mPrevPos.x;
            mPos.y = mPrevPos.y;
            mPos.z = mPrevPos.z;
            mFarLatch = 1;
        }
    } else {
        mFarLatch = 0;
    }

    if (mWithMeshClsn.IsOnWall() != 0) {
        if (mWallLatch == 0 && mFarLatch == 0 && mProbeLatch == 0) {
            ha = Vec3_HorzAngle(&mPos, &mAnchorPos);
            sh = (rnd & 3) << 0xc;
            mTargetAngleY = ha;
            p39a = &mTargetAngleY;
            angleSet = 1;
            *p39a = *p39a + (0x1800 - sh);
            mPos.x = mPrevPos.x;
            mPos.y = mPrevPos.y;
            mPos.z = mPrevPos.z;
            mWallLatch = 1;
        }
    } else {
        mWallLatch = 0;
    }

    if (mWanderTimer == 0 && mWallLatch == 0 && mFarLatch == 0 && mProbeLatch == 0) {
        mWanderTimer = (u16)((rnd + 50) & 0x3f);
        p39a = &mTargetAngleY;
        *p39a = *p39a + (0x1800 - ((rnd & 3) << 0xc));
    }

    if (angleSet == 1) {
        mTurnTimer = 10;
        mWanderTimer = 50;
        mSpeedHoldTimer = 30;
        mHorzSpeed = 0x9000;
        mAnimSpeed = 0x1000;
    }

    ApproachLinear(mPrevAngle.y, mTargetAngleY, 0x500);

    if (mTurnTimer == 0 && mSpeedHoldTimer == 0) {
        mAnimSpeed = 0x1000;
        mHorzSpeed = 0x9000;
        if (func_ov090_021314a0() == 1) {
            mSpeedHoldTimer = 30;
            mWanderTimer = 30;
            mAnimSpeed = 0x2000;
            mHorzSpeed = 0xe000;
        }
    }

    if (((u32)(((u32)mAnmModel.currFrame) << 4) >> 0x10) >= 0x10) {
        p390 = &mAnimLoops;
        *p390 = *p390 + 1;
    }

    if (mAnimLoops > 0x1e && mFarLatch == 0) {
        func_ov090_02131e00(data_ov090_021344e4);
    }

    if (mOnWater == 1) {
        func_ov090_02131e00(data_ov090_02134504);
    }

    return 1;
}

/* Patrol entry. */
int daMenbo_c::func_ov090_02131a74()
{
    mAnimSpeed = 0x1000;
    mSpeedHoldTimer = 0;
    mWanderTimer = 50;
    mAnimLoops = 0;
    mAnmModel.SetAnim((BCA_File *)data_ov090_02134480.filePtr, 0, 0x1000, 0);
    return 1;
}

/* Water rest. Bleed the speed off. Once it is gone, and the skeeter is not
 * on the water, drop the anchor here, pick a random heading and go idle.
 * When the rest timer expires, start the water chase. The heading keeps
 * easing either way. */
int daMenbo_c::func_ov090_02131ac4()
{
    ApproachLinear(mHorzSpeed, 0, 0x1000);
    if (mHorzSpeed > 0) return 1;
    if (mOnWater == 0) {
        mAnchorPos.x = mPos.x;
        mAnchorPos.y = mPos.y;
        mAnchorPos.z = mPos.z;
        mTargetAngleY = (u16)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) << 0xd);
        this->func_ov090_02131e00(data_ov090_021344e4);
    }
    ApproachAngle(&mPrevAngle.y, mTargetAngleY, 1, 0x100, 0x100);
    if (mStateTimer == 0)
        this->func_ov090_02131e00(data_ov090_021344f4);
    return 1;
}

/* Water-rest entry. Two random draws jitter the heading, a third sets how
 * long the rest lasts, then the rest anim starts. */
int daMenbo_c::func_ov090_02131b94()
{
    unsigned int r;
    short *s;
    r = (unsigned)RandomIntInternal(&data_0209e650.value);
    s = &mTargetAngleY;
    *s = (*s + ((int)(((r >> 8) & 3) << 0x1e) >> 16));
    r = (unsigned)RandomIntInternal(&data_0209e650.value);
    s = &mTargetAngleY;
    *s = (*s + ((int)(((r >> 8) & 7) << 0x1d) >> 16));
    r = (unsigned)RandomIntInternal(&data_0209e650.value);
    mStateTimer = (short)(((r >> 8) & 0x1f) + 150);
    mAnimSpeed = 0x1000;
    mAnmModel.SetAnim((BCA_File *)data_ov090_02134490.filePtr, 0, 0x1000, 0);
    return 1;
}

/* Water chase. Full speed after a few frames, and a wall close to the
 * heading kicks the target by a quarter turn. When the anim has looped
 * enough, go back to water rest. Off the water, drop the anchor and idle. */
int daMenbo_c::func_ov090_02131c48()
{
    if ((((unsigned)mAnmModel.currFrame) << 4) >> 16 >= 4)
        mHorzSpeed = 0x19000;

    if ((((unsigned)mAnmModel.currFrame) << 4) >> 16 == 4)
        func_02012694(0xfd, &mCamSpacePos);

    if (mTurnTimer == 0 && mWithMeshClsn.IsOnWall() &&
        AngleDiff(mPrevAngle.y, mTargetAngleY) < 0x200) {
        mTargetAngleY += 0x4000;
        mTurnTimer = 8;
    }

    if (mTurnTimer != 0) {
        ApproachAngle(&mPrevAngle.y, mTargetAngleY, 1, 0x1000, 0x1000);
    } else {
        if (mAnmModel.Finished()) {
            mAnimLoops++;
            if (mAnimLoops > 0x14)
                this->func_ov090_02131e00(data_ov090_02134504);
        }
    }

    if (mOnWater == 0) {
        mAnchorPos.x = mPos.x;
        mAnchorPos.y = mPos.y;
        mAnchorPos.z = mPos.z;
        mTargetAngleY = (u16)(((unsigned)RandomIntInternal(&data_0209e650.value) >> 8) << 13);
        this->func_ov090_02131e00(data_ov090_021344e4);
    }

    return 1;
}

/* Water-chase entry. The 0x40000000 flag is passed straight through. */
int daMenbo_c::func_ov090_02131db0()
{
    mAnmModel.SetAnim((BCA_File *)data_ov090_02134488.filePtr, 0x40000000, 0x1000, 0);
    mAnimSpeed = 0x1000;
    mAnimLoops = 0;
    return 1;
}

/* Install a state and run its entry. A null entry PMF means there is none. */
int daMenbo_c::func_ov090_02131e00(StateFn *state) {
    mState = state;
    if (*mState == 0)
        return 1;
    return (this->**mState)();
}

/* dExtSimpleModel_c matrix from position (shifted down 3, the renderer's scale) and
 * the three rotation halfwords. */
void daMenbo_c::func_ov090_02131e50() {
    int src[3];
    int dst[3];
    src[0] = mPos.x;
    src[1] = mPos.y;
    src[2] = mPos.z;
    Vec3_Asr(dst, src, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, dst[0], dst[1], dst[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngle.x, mAngle.y, mAngle.z);
    mAnmModel.mat4x3 = data_020a0e68;
}

/* The model and the four anims, released in this order. */
int daMenbo_c::CleanupResources()
{
    data_ov090_021344a0.Release();
    data_ov090_02134488.Release();
    data_ov090_02134480.Release();
    data_ov090_02134490.Release();
    data_ov090_02134498.Release();
    return 1;
}

/* Empty. The override is what keeps the base from running. */
void daMenbo_c::OnPendingDestroy()
{
}

/* Skip the frame while the Yoshi-mouth flag is set. */
int daMenbo_c::Render()
{
    unsigned int f = mFlags;
    int b = ((f & 0x40000) != 0);
    if (b) return 1;
    mAnmModel.Render(&mScale);
    return 1;
}

/* Yoshi's mouth, a killing blow, the death tumble, or one ordinary frame.
 * Course 0x15 area 1 treats a water hit on the mesh as a knock into the
 * water: motion zeroed, mDeathState = 1. Dying at or below the water line
 * pays the coins out. The cylinder stays quiet while the closest player
 * is vanished. */
int daMenbo_c::Behavior()
{
    if (UpdateYoshiEat(mWithMeshClsn)) {
        mdCcAcPos_c.Clear();
        if (mEatenByYoshi != 0 && mSpitOutTimer == 0)
            mdCcAcPos_c.Update();
        func_ov090_02131e50();
        return 1;
    }

    if (UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 3))
        return 1;

    if (mDeathState != 0) {
        mWithMeshClsn.SetHeight(0xd2000);
        UpdateWMClsn(mWithMeshClsn, 0);
        if (UpdateDeath(mWithMeshClsn))
            return 1;
        func_ov090_02131378();
        func_ov090_02131e50();
        if (mDeathState == 0)
            PoofDust();
        if (mDeathTumble == 3) {
            ApproachLinear(mAngle.x, -32767, 0x500);
            if (AngleDiff(mAngle.x, -32767) < 0x1000) {
                mAngle.y += 0x1000;
            }
        }
        if (mDeathState != 1 && mPos.y <= mSurfaceY) {
            Vector3 v;
            v.x = mPos.x;
            v.y = mPos.y;
            v.z = mPos.z;
            SpawnCoins(v, mExtraCoinCount + 1, 0xa000, 0);
            PoofDust();
            KillAndTrackInDeathTable();
        }
        return 1;
    }

    {
    int flag = (mFlags & 8) != 0;
    if (flag) {
        mHorzSpeed = 0;
        UpdatePos(&mdCcAcPos_c);
        func_ov090_02131378();
        if (gCurrentSublevel == 0x15 && mAreaId == 1) {
            UpdateWMClsn(mWithMeshClsn, 2);
            if (func_02035638((u8 *)(&mWithMeshClsn))) {
                mVertAccel = 0;
                mSpeed.x = 0;
                mSpeed.y = 0;
                mSpeed.z = 0;
                mDeathState = 1;
                func_ov002_020aea30(ClosestPlayer(), 0);
                return 1;
            }
        }
        return 1;
    }
    }

    UpdatePos(&mdCcAcPos_c);
    func_ov090_02131378();
    DecIfAbove0_Short(&mStateTimer);
    DecIfAbove0_Short(&mTurnTimer);
    DecIfAbove0_Short(&mSpeedHoldTimer);
    DecIfAbove0_Short(&mWanderTimer);
    UpdateWMClsn(mWithMeshClsn, 2);
    if (mPos.y <= mSurfaceY)
        mPos.y = mSurfaceY;
    if (gCurrentSublevel == 0x15 && mAreaId == 1 && func_02035638((u8 *)(&mWithMeshClsn))) {
        mVertAccel = 0;
        mSpeed.x = 0;
        mSpeed.y = 0;
        mSpeed.z = 0;
        mDeathState = 1;
        func_ov002_020aea30(ClosestPlayer(), 0);
        return 1;
    }

    if (mState[1])
        (this->*mState[1])();
    mAngle.y = mPrevAngle.y;
    mAnmModel.speed = mAnimSpeed;
    mAnmModel.Advance();
    func_ov090_02131e50();
    func_ov090_021310b4();
    mdCcAcPos_c.Clear();
    {
        daPly_c *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0)
            mdCcAcPos_c.Update();
    }
    return 1;
}

/* dExtSimpleModel_c, four anims, both collision volumes, then the water line.
 * Mode 2 on course 0x12 skips the raycast and starts at water chase with
 * the line equal to the spawn height. A surface whose 0x20 flag is set
 * records the hit as the line only and also starts at water chase. Anything
 * else snaps Y to the line, randomises the heading and starts idle. */
int daMenbo_c::InitResources()
{
    BMD_File *f;
    int r;
    Vector3 pos;
    Vector3 v;

    f = dExtSimpleModel_c::LoadFile(data_ov090_021344a0);
    mAnmModel.SetFile(f, 1, -1);
    dExtFrameCtrl_c::LoadFile(data_ov090_02134488);
    dExtFrameCtrl_c::LoadFile(data_ov090_02134480);
    dExtFrameCtrl_c::LoadFile(data_ov090_02134490);
    dExtFrameCtrl_c::LoadFile(data_ov090_02134498);

    mTerminalVelocity = -0x3c000;

    v = data_ov090_0213412c;
    mdCcAcPos_c.Init(this, v, 0x5a000, 0x5a000, 0x200000, 0x7eff0);

    mAngle.y = mPrevAngle.y;
    mAnimSpeed = 0x1000;
    mScale.x = 0x1000;
    mScale.y = 0x1000;
    mScale.z = 0x1000;
    mWithMeshClsn.Init(this, 0xc8000, 0, 0, 0);
    mWithMeshClsn.SetNoUpdatePosYFlag();

    mCoinType = 1;
    mExtraCoinCount = 2;
    mVertAccel = -0x3000;

    if (IsGameMode(2) && gCurrentSublevel == 0x12) {
        mSurfaceY = mPos.y;
        mAnchorPos.x = mPos.x;
        mAnchorPos.y = mPos.y;
        mAnchorPos.z = mPos.z;
        this->func_ov090_02131e00(data_ov090_021344f4);
        return 1;
    }

    {
        dBgCh_Gnd ground;
        ground.mProbeHeight = 0xbb8000;
        ground.StartDetectingWater();
        {
            int py = mPos.y;
            int pz = mPos.z;
            int px = mPos.x;
            int ip = py + 0x32000;
            pos.x = px;
            pos.y = ip;
            pos.z = pz;
        }
        ground.SetObjAndPos(pos, this);
        mFloorY = data_02092138;
        if (ground.DetectClsn() != 0) {
            if (SurfaceInfo_TestFlag0x20((int *)&ground.surface) != 0) {
                mOnWater = 1;
                mSurfaceY = ground.clsnY;
            } else {
                mFloorY = ground.clsnY;
                mSurfaceY = ground.clsnY;
            }
        }

        mPos.y = mSurfaceY;
        mAnchorPos.x = mPos.x;
        mAnchorPos.y = mPos.y;
        mAnchorPos.z = mPos.z;

        if (mOnWater != 0) {
            this->func_ov090_02131e00(data_ov090_021344f4);
            return 1;
        }

        {
            r = RandomIntInternal(&data_0209e650.value);
            short ang = (short)((((unsigned int)r >> 8) & 0xf) << 12);
            mTargetAngleY = ang;
            mPrevAngle.y = mTargetAngleY;
            mAngle.y = mPrevAngle.y;
        }
        this->func_ov090_02131e00(data_ov090_021344e4);
    }

    return 1;
}

/* How far Yoshi's aim leads this target. */
s32 daMenbo_c::OnAimedAtWithEgg()
{
    return 0x20000;
}

void daMenbo_c::OnTurnIntoEgg(daPly_c &player) {
    GivePlayerCoins(player, (mExtraCoinCount + 1), 0);
    KillAndTrackInDeathTable();
}

/* What Yoshi turns this into when he swallows it. */
s32 daMenbo_c::OnYoshiTryEat()
{
    return 4;
}

/* The five file handles and the four state nodes. mwccarm emits
   __sinit_d_a_menbo.cpp from these definitions: construct+register for each
   handle, then the eight PMF descriptor copies. */
MenboModelFilePtr     data_ov090_021344a0(0x39f);
MenboAnimationFilePtr data_ov090_02134488(0x3a0);
MenboAnimationFilePtr data_ov090_02134480(0x3a1);
MenboAnimationFilePtr data_ov090_02134490(0x3a2);
MenboAnimationFilePtr data_ov090_02134498(0x3a3);
daMenbo_c::StateFn data_ov090_021344f4[2] = {
    &daMenbo_c::func_ov090_02131db0,
    &daMenbo_c::func_ov090_02131c48,
};
daMenbo_c::StateFn data_ov090_02134504[2] = {
    &daMenbo_c::func_ov090_02131b94,
    &daMenbo_c::func_ov090_02131ac4,
};
daMenbo_c::StateFn data_ov090_02134514[2] = {
    &daMenbo_c::func_ov090_02131a74,
    &daMenbo_c::func_ov090_02131648,
};
daMenbo_c::StateFn data_ov090_021344e4[2] = {
    &daMenbo_c::func_ov090_02131608,
    &daMenbo_c::func_ov090_02131584,
};

/* Reconstructed source-style name: SM64DS proves daMenbo_c through RTTI,
 * allocation size, vtable identity, and the MENBO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Skeeter_Spawn.
 *
 * `new daMenbo_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x3b0), dEnemyBase_c's base constructor, the vptr
 * store, then the dCcAcPos_c, dBgCh_Actr and dExtAnmModel_c member constructors. */
daMenbo_c *daMenbo_c_classInit(void) {
    return new daMenbo_c;
}
