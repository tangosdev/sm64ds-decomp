//cpp
struct Vector3_16f;
namespace Particle {
struct Callback;
struct System {
    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
};
}
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
 * common.h is included first so Matrix4x3 is the flat twelve words the two
 * matrix copies were matched under. defer_codegen is off, so the bodies are
 * ROM-ascending. opt_strength_reduction stays off for the file;
 * opt_common_subs is off except around the ground probe and Behavior.
 *
 * deslop leftovers:
 * - func_ov090_02131ac4: `mStateTimer == 0` is ldrsh. The ROM is ldrh, one
 *   word. `*(unsigned short *)&mStateTimer` matches.
 * - func_ov090_02131608: dExtAnmModel_c::SetAnim with a Fix12<int> rate is 0x4c
 *   against 0x40 and emits 4 bytes of .rodata. The scalar SetAnim call
 *   matches. The patrol, water-rest and water-chase entries keep it.
 */

#pragma defer_codegen off

#include "common.h"
#include "daMenbo_c.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "dExtFrameCtrl_c.h"
#include "math/Approach.h"

#pragma opt_strength_reduction off
#pragma opt_common_subs off

struct BMD_File;
struct BCA_File;

/* mState is void* on the class. The node it points at is two PMFs on
 * daMenbo_c: the entry Behavior does not call, then the update it does.
 * MenboState is the pun the installer reads them through. */
typedef int (daMenbo_c::*MenboStateFn)();
struct MenboState { char pad[0x370]; MenboStateFn *mState; };

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

struct CoinVec3 { s32 x, y, z; };

/* By-value Fix12<int> in a mangled name; the calls pass a bare word. */
typedef int LocFix12;

extern "C" {

void  MulMat4x3Mat4x3(Matrix4x3 *a, Matrix4x3 *b, Matrix4x3 *out);
void  MulVec3Mat4x3(const Vector3 *v, const void *m, Vector3 *out);
void  Matrix4x3_FromRotationY(void *m, s16 angle);
void  Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, s16 rx, s16 ry, s16 rz);
void  Vec3_Lsl(Vector3 *out, const Vector3 *in, int n);
void  Vec3_Asr(void *dst, void *src, int n);
int   Vec3_Dist(const Vector3 *a, const Vector3 *b);
int   Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
s16   Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int   AngleDiff(int a, int b);
int   ApproachAngle(s16 *cur, int target, int divisor, int band, int maxStep);
u16   DecIfAbove0_Short(u16 *p);
int   RandomIntInternal(int *seed);
void  func_02012694(int id, const Vector3 *pos);

int d_a_menboNewHold(unsigned int a0, unsigned int a1, int a2, int a3, int a4, int a5, int a6);

/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(dBgCh_Actr *self, int a, LocFix12 r, LocFix12 h, int p, int q);
int   SurfaceInfo_TestFlag0x20(const SurfaceInfo *p);
void  func_0203558c(void *self);
int   func_02035638(u8 *p);
void  func_02035684(int *p, int v);

void  func_ov002_020aea30(void *self, void *actor, void *collision);

extern Matrix4x3     data_020a0e68;
extern int           data_02092138;
extern int           data_0209e650;
extern unsigned char data_0209f2d8;
extern signed char   data_0209f2f8;
extern int           data_0209f32c;

extern Vector3       data_ov090_0213412c;
extern MenboAnimationFilePtr data_ov090_02134480; /* patrol anim, file 0x3a1 */
extern MenboAnimationFilePtr data_ov090_02134488; /* water-chase anim, file 0x3a0 */
extern MenboAnimationFilePtr data_ov090_02134490; /* water-rest anim, file 0x3a2 */
extern MenboAnimationFilePtr data_ov090_02134498; /* idle anim, file 0x3a3 */
extern MenboModelFilePtr     data_ov090_021344a0; /* model, file 0x39f */
/* Each node is the (entry, update) PMF pair the Behavior installs.
 * e4 idle, f4 water chase, 504 water rest, 514 patrol. */
extern MenboStateFn  data_ov090_021344e4[2];
extern MenboStateFn  data_ov090_021344f4[2];
extern MenboStateFn  data_ov090_02134504[2];
extern MenboStateFn  data_ov090_02134514[2];

void func_ov090_021310b4(daMenbo_c *c);
void func_ov090_02131378(daMenbo_c *c);
int  func_ov090_021314a0(daMenbo_c *c);
int  func_ov090_02131e00(MenboState *c, MenboStateFn *p);
void func_ov090_02131e50(daMenbo_c *c);

}

/* dActor_c::mFlags bit 0x08 is written by the framework while the actor is
 * off screen. Actor ID 0xbf is PLAYER. data_0209f2d8 is the current game
 * mode. These int-returning predicates are what make the ROM materialize
 * 0/1 before testing it. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

static inline int IsGameMode(int mode) {
    return data_0209f2d8 == mode;
}

// @symbol _ZN9daMenbo_cD1Ev
// @symbol _ZN9daMenbo_cD0Ev
daMenbo_c::~daMenbo_c()
{
}

// @symbol func_ov090_02130f94
/* One ripple under each of four leg bones. Skipped while the actor is off
 * screen. Bone matrices are transforms[6], [9], [12] and [15], multiplied
 * by the model matrix; the ripple sits on the water line. */
extern "C" void func_ov090_02130f94(daMenbo_c *c_)
{
    int zero;
    int sh;
    daMenbo_c *c;
    int i;
    Matrix4x3 *src;
    int idx;
    unsigned int id;

    c = c_;
    if (IsOffScreen(c))
        return;

    src = &c->mAnmModel.mat4x3;
    i = 0;
    idx = 6;
    zero = 0;
    sh = 3;
    id = 0xea;
    for (; i < 4; ++i) {
        Vector3 v;
        Vector3 r;

        data_020a0e68 = *src;
        MulMat4x3Mat4x3((Matrix4x3 *)c->mAnmModel.data.transforms + idx, &data_020a0e68,
                        &data_020a0e68);
        v.x = *(int *)((char *)&data_020a0e68 + 0x24);
        v.y = *(int *)((char *)&data_020a0e68 + 0x28);
        v.z = *(int *)((char *)&data_020a0e68 + 0x2c);
        Vec3_Lsl(&r, &v, sh);
        v.x = r.x;
        v.y = r.y;
        v.z = r.z;
        v.y = c->mSurfaceY;
        c->mRippleParticles[i] =
            Particle::System::New(c->mRippleParticles[i], id, v.x, v.y, r.z,
                                  (const Vector3_16f *)(zero), (Particle::Callback *)(zero));
        idx += 3;
    }
}

// @symbol func_ov090_021310b4
/* Contact response, in the order the tests run. A volatile read of the
 * cylinder offset keeps that copy as three loads. */
void func_ov090_021310b4(daMenbo_c *c)
{
    short hv[3];
    Vector3 sv;
    Vector3 cv;
    Vector3 hurt;
    int flags;
    dActor_c *p;
    int x, y, z;
    volatile Vector3 *src = &data_ov090_0213412c;

    x = src->x;
    y = src->y;
    z = src->z;
    sv.z = z;
    sv.x = x;
    sv.y = y;
    c->mdCcAcPos_c.SetPosRelativeToActor(sv);

    if (c->mdCcAcPos_c.otherOwner == 0) return;
    if ((p = dActor_c::FindWithID(c->mdCcAcPos_c.otherOwner)) == 0) return;
    flags = (int)((long long)*(int *)&c->mdCcAcPos_c.hitFlags);

    if (flags & 0x2400) {
        c->mDeathState = 2;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x40000) {
        c->mDeathState = 4;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x40) {
        c->mDeathState = 2;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (c->JumpedOnByPlayer(*(dCc_c *)&c->mdCcAcPos_c, *(Player *)p)) {
        ((Player *)p)->Bounce(0x28000);
        c->mDeathState = 1;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x20) {
        c->mVertAccel = 0;
        c->mSpeedX = 0;
        c->mVertSpeed = 0;
        c->mSpeedZ = 0;
        c->mDeathState = 1;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x10) {
        hv[0] = 0x1000;
        hv[1] = 0;
        hv[2] = 0;
        c->KillByInvincibleChar(*(Vector3_16 *)hv, *(Player *)p, 0);
        func_02012694(0x1d, &c->mCamSpacePos);
        return;
    }
    if (flags & 0x4380) {
        c->mDeathTumble = 3;
        c->mDeathState = 3;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (!IsPlayerActor(p))
        return;
    if (((Player *)p)->mIsMetal == 1 || ((Player *)p)->IsOnShell() == 1) {
        cv.x = c->mPosX;
        cv.y = c->mPosY;
        cv.z = c->mPosZ;
        c->SpawnCoins(cv, c->mExtraCoinCount + 1, 0xa000, 0);
        c->PoofDust();
        c->KillAndTrackInDeathTable();
        return;
    }
    hurt.x = c->mPosX;
    hurt.y = c->mPosY;
    hurt.z = c->mPosZ;
    ((Player *)p)->Hurt(hurt, 2, 0xc000, 1, 0, 1);
}

#pragma opt_common_subs on
// @symbol func_ov090_02131378
/* Ground probe. Mode 2 on course 0x12 skips it and only spawns ripples.
 * Otherwise a hit below the published water height (data_0209f32c) means
 * the skeeter is on the water: record that line, mark mOnWater, ripple.
 * A higher floor is the line instead, and either way Y is clamped up to it. */
void func_ov090_02131378(daMenbo_c *c)
{
    if (IsGameMode(2) && data_0209f2f8 == 0x12) {
        func_ov090_02130f94(c);
        return;
    }
    dBgCh_Gnd rg;
    Vector3 v;
    int vz = c->mPosZ;
    int vy = c->mPosY + 0x32000;
    v.x = c->mPosX;
    v.y = vy;
    v.z = vz;
    c->mOnWater = 0;
    if (Vec3_HorzDist(&c->mPos, &c->mPrevPos) != 0) {
        rg.SetObjAndPos(v, c);
        if (rg.DetectClsn() != 0)
            c->mFloorY = rg.clsnY;
    }
    if (c->mFloorY < data_0209f32c) {
        c->mSurfaceY = data_0209f32c;
        if (c->mPosY <= c->mSurfaceY) {
            c->mOnWater = 1;
            func_ov090_02130f94(c);
        }
    } else {
        c->mSurfaceY = c->mFloorY;
    }
    if (c->mPosY <= c->mSurfaceY) {
        c->mPosY = c->mSurfaceY;
        c->mVertSpeed = 0;
    }
}
#pragma opt_common_subs off

// @symbol func_ov090_021314a0
/* 1 while a player is close and within a quarter turn of the heading.
 * The held target latches; releasing it points the skeeter back at its
 * anchor and sets the patrol speed. */
int func_ov090_021314a0(daMenbo_c *c)
{
    Player *p = c->ClosestNonVanishPlayer();
    if (p != 0 && AngleDiff(c->mPrevAngleY, c->HorzAngleToCPlayer()) < 0x2000) {
        int *sv = &p->mPosX;
        int v[3];
        v[0] = sv[0];
        v[1] = sv[1];
        v[2] = sv[2];
        if (Vec3_Dist(&c->mPos, (const Vector3 *)v) < 0x3ac000) {
            int w[3];
            w[0] = v[0];
            w[1] = v[1];
            w[2] = v[2];
            c->mTargetAngleY = (u16)Vec3_HorzAngle(&c->mPos, (const Vector3 *)w);
            c->mChaseLatch = 1;
            return 1;
        }
    }
    if (c->mChaseLatch == 1) {
        c->mHorzSpeed = 0x9000;
        c->mChaseLatch = 0;
        c->mTargetAngleY = (u16)Vec3_HorzAngle(&c->mPos, (const Vector3 *)&c->mAnchorPosX);
    }
    return 0;
}

// @symbol _ZN9daMenbo_c19func_ov090_02131584Ev
/* Idle. Speed stays zero. After a few loops of the idle anim, or as soon
 * as a player is in range, start patrolling. On the water, rest instead. */
int daMenbo_c::func_ov090_02131584()
{
    unsigned int v;
    mHorzSpeed = 0;
    v = ((unsigned int)static_cast<dExtFrameCtrl_c &>(mAnmModel).currFrame << 4) >> 0x10;
    if (v >= 0x3b)
        mAnimLoops += 1;
    if (mAnimLoops > 2 || func_ov090_021314a0(this) == 1)
        func_ov090_02131e00((MenboState *)this, data_ov090_02134514);
    if (mOnWater == 1)
        func_ov090_02131e00((MenboState *)this, data_ov090_02134504);
    return 1;
}

// @symbol _ZN9daMenbo_c19func_ov090_02131608Ev
/* Idle entry. */
int daMenbo_c::func_ov090_02131608()
{
    mAnimLoops = 0;
    mAnimSpeed = 0x1000;
    mAnmModel.SetAnim((BCA_File *)data_ov090_02134498.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN9daMenbo_c19func_ov090_02131648Ev
/* Patrol. A line probe ahead, the distance back to the anchor, and a wall
 * each turn it around, and each latches so the turn only fires on the edge.
 * A turn reloads the three timers and snaps the position back to last
 * frame's. The heading then eases toward mTargetAngleY. Close to a player, the
 * playback rate and the speed both double for a bit. */
int daMenbo_c::func_ov090_02131648()
{
    daMenbo_c *self = this;
    int dist;
    u32 rnd;
    int selfY;
    Vector3 a, b, in, out;
    int angleSet;
    s16 *p39a;
    int sh;
    s16 ha;
    s32 *p390;

    rnd = (u32)RandomIntInternal(&data_0209e650) >> 8;
    dist = Vec3_Dist(&self->mPos, (Vector3 *)&self->mAnchorPosX);

    if (((((u32) static_cast<dExtFrameCtrl_c &>(self->mAnmModel).currFrame) << 4) >> 0x10 & 0xf) ==
        0) {
        func_02012694(0xfc, &self->mCamSpacePos);
    }

    angleSet = 0;
    dBgCh_Lin line;

    a.x = 0; a.y = 0; a.z = 0;
    b.x = 0; b.y = 0; b.z = 0;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    a.x = self->mPosX;
    selfY = self->mPosY;
    a.y = selfY;
    a.z = self->mPosZ;
    a.y = selfY + 0x64000;
    in.y = 0x64000;
    in.z = 0x1f4000;

    Matrix4x3_FromRotationY(&data_020a0e68, self->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, 0x2000);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);

    b.x = a.x;
    b.x = a.x + out.x;
    b.y = a.y;
    b.z = a.z;
    b.y = a.y + out.y;
    b.z = a.z + out.z;

    line.SetObjAndLine(a, b, self);

    if (!line.DetectClsn()) {
        if (self->mProbeLatch == 0) {
            ha = Vec3_HorzAngle(&self->mPos, (Vector3 *)&self->mAnchorPosX);
            sh = (rnd & 3) << 0xc;
            self->mTargetAngleY = (u16)ha;
            p39a = (s16 *)&self->mTargetAngleY;
            angleSet = 1;
            *p39a = *p39a + (0x1800 - sh);
            self->mPosX = self->mPrevPosX;
            self->mPosY = self->mPrevPosY;
            self->mPosZ = self->mPrevPosZ;
            self->mProbeLatch = 1;
        }
    } else {
        self->mProbeLatch = (u8)angleSet;
    }

    if (dist > 0x3c0000) {
        if (self->mFarLatch == 0 && self->mProbeLatch == 0) {
            ha = Vec3_HorzAngle(&self->mPos, (Vector3 *)&self->mAnchorPosX);
            self->mTargetAngleY = (u16)ha;
            angleSet = 1;
            self->mPosX = self->mPrevPosX;
            self->mPosY = self->mPrevPosY;
            self->mPosZ = self->mPrevPosZ;
            self->mFarLatch = 1;
        }
    } else {
        self->mFarLatch = 0;
    }

    if (self->mWithMeshClsn.IsOnWall() != 0) {
        if (self->mWallLatch == 0 && self->mFarLatch == 0 && self->mProbeLatch == 0) {
            ha = Vec3_HorzAngle(&self->mPos, (Vector3 *)&self->mAnchorPosX);
            sh = (rnd & 3) << 0xc;
            self->mTargetAngleY = (u16)ha;
            p39a = (s16 *)&self->mTargetAngleY;
            angleSet = 1;
            *p39a = *p39a + (0x1800 - sh);
            self->mPosX = self->mPrevPosX;
            self->mPosY = self->mPrevPosY;
            self->mPosZ = self->mPrevPosZ;
            self->mWallLatch = 1;
        }
    } else {
        self->mWallLatch = 0;
    }

    if (self->mWanderTimer == 0 && self->mWallLatch == 0 && self->mFarLatch == 0 &&
        self->mProbeLatch == 0) {
        self->mWanderTimer = (u16)((rnd + 0x32) & 0x3f);
        p39a = (s16 *)&self->mTargetAngleY;
        *p39a = *p39a + (0x1800 - ((rnd & 3) << 0xc));
    }

    if (angleSet == 1) {
        self->mTurnTimer = 0xa;
        self->mWanderTimer = 0x32;
        self->mSpeedHoldTimer = 0x1e;
        self->mHorzSpeed = 0x9000;
        self->mAnimSpeed = 0x1000;
    }

    ApproachLinear(self->mPrevAngleY, (s16)self->mTargetAngleY, 0x500);

    if (self->mTurnTimer == 0 && self->mSpeedHoldTimer == 0) {
        self->mAnimSpeed = 0x1000;
        self->mHorzSpeed = 0x9000;
        if (func_ov090_021314a0(self) == 1) {
            self->mSpeedHoldTimer = 0x1e;
            self->mWanderTimer = 0x1e;
            self->mAnimSpeed = 0x2000;
            self->mHorzSpeed = 0xe000;
        }
    }

    if (((u32)(((u32) static_cast<dExtFrameCtrl_c &>(self->mAnmModel).currFrame) << 4) >> 0x10) >=
        0x10) {
        p390 = &self->mAnimLoops;
        *p390 = *p390 + 1;
    }

    if (self->mAnimLoops > 0x1e && self->mFarLatch == 0) {
        func_ov090_02131e00((MenboState *)self, data_ov090_021344e4);
    }

    if (self->mOnWater == 1) {
        func_ov090_02131e00((MenboState *)self, data_ov090_02134504);
    }

    return 1;
}

// @symbol _ZN9daMenbo_c19func_ov090_02131a74Ev
/* Patrol entry. */
int daMenbo_c::func_ov090_02131a74()
{
    mAnimSpeed = 0x1000;
    mSpeedHoldTimer = 0;
    mWanderTimer = 0x32;
    mAnimLoops = 0;
    mAnmModel.SetAnim((BCA_File *)data_ov090_02134480.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN9daMenbo_c19func_ov090_02131ac4Ev
/* Water rest. Bleed the speed off. Once it is gone, and the skeeter is not
 * on the water, drop the anchor here, pick a random heading and go idle.
 * When the rest timer expires, start the water chase. The heading keeps
 * easing either way. */
int daMenbo_c::func_ov090_02131ac4()
{
    ApproachLinear(mHorzSpeed, 0, 0x1000);
    if (mHorzSpeed > 0) return 1;
    if (mOnWater == 0) {
        mAnchorPosX = mPosX;
        mAnchorPosY = mPosY;
        mAnchorPosZ = mPosZ;
        mTargetAngleY = (u16)(((unsigned int)RandomIntInternal(&data_0209e650) >> 8) << 0xd);
        func_ov090_02131e00((MenboState *)this, data_ov090_021344e4);
    }
    ApproachAngle(&mPrevAngleY, (s16)mTargetAngleY, 1, 0x100, 0x100);
    if (*(unsigned short *)&mStateTimer == 0)
        func_ov090_02131e00((MenboState *)this, data_ov090_021344f4);
    return 1;
}

// @symbol _ZN9daMenbo_c19func_ov090_02131b94Ev
/* Water-rest entry. Two random draws jitter the heading, a third sets how
 * long the rest lasts, then the rest anim starts. */
int daMenbo_c::func_ov090_02131b94()
{
    unsigned int r;
    short *s;
    r = (unsigned)RandomIntInternal(&data_0209e650);
    s = (short *)&mTargetAngleY;
    *s = (short)(*s + ((int)(((r >> 8) & 3) << 0x1e) >> 16));
    r = (unsigned)RandomIntInternal(&data_0209e650);
    s = (short *)&mTargetAngleY;
    *s = (short)(*s + ((int)(((r >> 8) & 7) << 0x1d) >> 16));
    r = (unsigned)RandomIntInternal(&data_0209e650);
    mStateTimer = (short)(((r >> 8) & 0x1f) + 0x96);
    mAnimSpeed = 0x1000;
    mAnmModel.SetAnim((BCA_File *)data_ov090_02134490.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN9daMenbo_c19func_ov090_02131c48Ev
/* Water chase. Full speed after a few frames, and a wall close to the
 * heading kicks the target by a quarter turn. When the anim has looped
 * enough, go back to water rest. Off the water, drop the anchor and idle. */
int daMenbo_c::func_ov090_02131c48()
{
    if ((((unsigned)static_cast<dExtFrameCtrl_c &>(mAnmModel).currFrame) << 4) >> 16 >= 4)
        mHorzSpeed = 0x19000;

    if ((((unsigned)static_cast<dExtFrameCtrl_c &>(mAnmModel).currFrame) << 4) >> 16 == 4)
        func_02012694(0xfd, &mCamSpacePos);

    if (mTurnTimer == 0 && mWithMeshClsn.IsOnWall() &&
        AngleDiff(mPrevAngleY, (s16)mTargetAngleY) < 0x200) {
        s16 *p = (s16 *)&mTargetAngleY;
        *p = *p + 0x4000;
        mTurnTimer = 8;
    }

    if (mTurnTimer != 0) {
        ApproachAngle(&mPrevAngleY, (s16)mTargetAngleY, 1, 0x1000, 0x1000);
    } else {
        if (static_cast<dExtFrameCtrl_c &>(mAnmModel).Finished()) {
            s32 *q = &mAnimLoops;
            *q = *q + 1;
            if (mAnimLoops > 0x14)
                func_ov090_02131e00((MenboState *)this, data_ov090_02134504);
        }
    }

    if (mOnWater == 0) {
        mAnchorPosX = mPosX;
        mAnchorPosY = mPosY;
        mAnchorPosZ = mPosZ;
        mTargetAngleY = (u16)(((unsigned)RandomIntInternal(&data_0209e650) >> 8) << 13);
        func_ov090_02131e00((MenboState *)this, data_ov090_021344e4);
    }

    return 1;
}

// @symbol _ZN9daMenbo_c19func_ov090_02131db0Ev
/* Water-chase entry. The 0x40000000 flag is passed straight through. */
int daMenbo_c::func_ov090_02131db0()
{
    mAnmModel.SetAnim((BCA_File *)data_ov090_02134488.filePtr, 0x40000000, 0x1000, 0);
    mAnimSpeed = 0x1000;
    mAnimLoops = 0;
    return 1;
}

// @symbol func_ov090_02131e00
/* Install a state and run its entry. A null entry PMF means there is none. */
int func_ov090_02131e00(MenboState *c, MenboStateFn *p)
{
    c->mState = p;
    MenboStateFn *q = c->mState;
    if (*q == 0) return 1;
    return ((daMenbo_c *)c->**q)();
}

// @symbol func_ov090_02131e50
/* dExtSimpleModel_c matrix from position (shifted down 3, the renderer's scale) and
 * the three rotation halfwords. */
void func_ov090_02131e50(daMenbo_c *c)
{
    int src[3];
    int dst[3];
    src[0] = c->mPosX;
    src[1] = c->mPosY;
    src[2] = c->mPosZ;
    Vec3_Asr(dst, src, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, dst[0], dst[1], dst[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);
    c->mAnmModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN9daMenbo_c16CleanupResourcesEv
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

// @symbol _ZN9daMenbo_c16OnPendingDestroyEv
/* Empty. The override is what keeps the base from running. */
void daMenbo_c::OnPendingDestroy()
{
}

// @symbol _ZN9daMenbo_c6RenderEv
/* Skip the frame while the Yoshi-mouth flag is set. */
int daMenbo_c::Render()
{
    unsigned int f = mFlags;
    int b = ((f & 0x40000) != 0);
    if (b) return 1;
    mAnmModel.Render(&mScale);
    return 1;
}

#pragma opt_common_subs on
// @symbol _ZN9daMenbo_c8BehaviorEv
/* Yoshi's mouth, a killing blow, the death tumble, or one ordinary frame.
 * Course 0x15 area 1 treats a water hit on the mesh as a knock into the
 * water: motion zeroed, mDeathState = 1. Dying at or below the water line
 * pays the coins out. The cylinder stays quiet while the closest player
 * is vanished. */
int daMenbo_c::Behavior()
{
    char *c = (char *)this;

    if (UpdateYoshiEat(mWithMeshClsn)) {
        mdCcAcPos_c.Clear();
        if (mEatenByYoshi != 0 && mSpitOutTimer == 0)
            mdCcAcPos_c.Update();
        func_ov090_02131e50(this);
        return 1;
    }

    if (UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 3))
        return 1;

    if (mDeathState != 0) {
        func_02035684((int *)(&mWithMeshClsn), 0xd2000);
        UpdateWMClsn(mWithMeshClsn, 0);
        if (UpdateDeath(mWithMeshClsn))
            return 1;
        func_ov090_02131378(this);
        func_ov090_02131e50(this);
        if (mDeathState == 0)
            PoofDust();
        if (mDeathTumble == 3) {
            ApproachLinear(mAngleX, -32767, 0x500);
            if (AngleDiff(*&mAngleX, -32767) < 0x1000) {
                s16 *yaw = &mAngleY;
                *yaw += 0x1000;
            }
        }
        if (mDeathState != 1 && mPosY <= mSurfaceY) {
            CoinVec3 v;
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            ((dActor_c *)c)->SpawnCoins(*(const Vector3 *)&v, mExtraCoinCount + 1, 0xa000, 0);
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
        func_ov090_02131378(this);
        if (data_0209f2f8 == 0x15 && mAreaId == 1) {
            UpdateWMClsn(mWithMeshClsn, 2);
            if (func_02035638((u8 *)(&mWithMeshClsn))) {
                mVertAccel = 0;
                mSpeedX = 0;
                mVertSpeed = 0;
                mSpeedZ = 0;
                mDeathState = 1;
                func_ov002_020aea30(c, ClosestPlayer(), 0);
                return 1;
            }
        }
        return 1;
    }
    }

    UpdatePos(&mdCcAcPos_c);
    func_ov090_02131378(this);
    DecIfAbove0_Short((u16 *)&mStateTimer);
    DecIfAbove0_Short(&mTurnTimer);
    DecIfAbove0_Short(&mSpeedHoldTimer);
    DecIfAbove0_Short(&mWanderTimer);
    UpdateWMClsn(mWithMeshClsn, 2);
    if (mPosY <= mSurfaceY)
        mPosY = mSurfaceY;
    if (data_0209f2f8 == 0x15 && mAreaId == 1 && func_02035638((u8 *)(&mWithMeshClsn))) {
        mVertAccel = 0;
        mSpeedX = 0;
        mVertSpeed = 0;
        mSpeedZ = 0;
        mDeathState = 1;
        func_ov002_020aea30(c, ClosestPlayer(), 0);
        return 1;
    }

    {
        MenboStateFn *n = (MenboStateFn *)mState;
        if (n[1])
            (this->*n[1])();
    }
    mAngleY = mPrevAngleY;
    static_cast<dExtFrameCtrl_c &>(mAnmModel).speed = mAnimSpeed;
    static_cast<dExtFrameCtrl_c &>(mAnmModel).Advance();
    func_ov090_02131e50(this);
    func_ov090_021310b4(this);
    mdCcAcPos_c.Clear();
    {
        Player *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0)
            mdCcAcPos_c.Update();
    }
    return 1;
}
#pragma opt_common_subs off

// @symbol _ZN9daMenbo_c13InitResourcesEv
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

    f = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov090_021344a0);
    mAnmModel.SetFile(f, 1, -1);
    dExtFrameCtrl_c::LoadFile(data_ov090_02134488);
    dExtFrameCtrl_c::LoadFile(data_ov090_02134480);
    dExtFrameCtrl_c::LoadFile(data_ov090_02134490);
    dExtFrameCtrl_c::LoadFile(data_ov090_02134498);

    mTerminalVelocity = -0x3c000;

    v.x = data_ov090_0213412c.x;
    v.y = data_ov090_0213412c.y;
    v.z = data_ov090_0213412c.z;
    mdCcAcPos_c.Init(this, v, 0x5a000, 0x5a000, 0x200000, 0x7eff0);

    mAngleY = mPrevAngleY;
    mAnimSpeed = 0x1000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mWithMeshClsn.Init((dActor_c *)(int)this, 0xc8000, 0, 0, 0);
    func_0203558c(&mWithMeshClsn);

    mCoinType = 1;
    mExtraCoinCount = 2;
    mVertAccel = -0x3000;

    if (IsGameMode(2) && data_0209f2f8 == 0x12) {
        mSurfaceY = mPosY;
        mAnchorPosX = mPosX;
        mAnchorPosY = mPosY;
        mAnchorPosZ = mPosZ;
        func_ov090_02131e00((MenboState *)this, data_ov090_021344f4);
        return 1;
    }

    {
        dBgCh_Gnd ground;
        ground.mProbeHeight = 0xbb8000;
        ground.StartDetectingWater();
        {
            int py = mPosY;
            int pz = mPosZ;
            int px = mPosX;
            int ip = py + 0x32000;
            pos.x = px;
            pos.y = ip;
            pos.z = pz;
        }
        ground.SetObjAndPos(pos, this);
        mFloorY = data_02092138;
        if (ground.DetectClsn() != 0) {
            if (SurfaceInfo_TestFlag0x20(&ground.surface) != 0) {
                mOnWater = 1;
                mSurfaceY = ground.clsnY;
            } else {
                mFloorY = ground.clsnY;
                mSurfaceY = ground.clsnY;
            }
        }

        mPosY = mSurfaceY;
        mAnchorPosX = mPosX;
        mAnchorPosY = mPosY;
        mAnchorPosZ = mPosZ;

        if (mOnWater != 0) {
            func_ov090_02131e00((MenboState *)this, data_ov090_021344f4);
            return 1;
        }

        {
            r = RandomIntInternal(&data_0209e650);
            short ang = (short)((((unsigned int)r >> 8) & 0xf) << 12);
            *(short *)&mTargetAngleY = ang;
            mPrevAngleY = *(short *)&mTargetAngleY;
            mAngleY = mPrevAngleY;
        }
        func_ov090_02131e00((MenboState *)this, data_ov090_021344e4);
    }

    return 1;
}

// @symbol _ZN9daMenbo_c16OnAimedAtWithEggEv
/* How far Yoshi's aim leads this target. */
s32 daMenbo_c::OnAimedAtWithEgg()
{
    return 0x20000;
}

// @symbol _ZN9daMenbo_c13OnTurnIntoEggER6Player
void daMenbo_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(mExtraCoinCount + 1), 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN9daMenbo_c13OnYoshiTryEatEv
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
MenboStateFn data_ov090_021344f4[2] = {
    &daMenbo_c::func_ov090_02131db0,
    &daMenbo_c::func_ov090_02131c48,
};
MenboStateFn data_ov090_02134504[2] = {
    &daMenbo_c::func_ov090_02131b94,
    &daMenbo_c::func_ov090_02131ac4,
};
MenboStateFn data_ov090_02134514[2] = {
    &daMenbo_c::func_ov090_02131a74,
    &daMenbo_c::func_ov090_02131648,
};
MenboStateFn data_ov090_021344e4[2] = {
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
// @symbol daMenbo_c_classInit
extern "C" daMenbo_c *daMenbo_c_classInit(void)
{
    return new daMenbo_c;
}
