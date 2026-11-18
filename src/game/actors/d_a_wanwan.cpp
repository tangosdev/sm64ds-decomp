//cpp
/**
 * daWanwan_c -- Bob-omb Battlefield Chain Chomp. Seven links, the stump it
 * spawns (STUMP 0x1b, daObjPile_c) and the fence it finds
 * (CHAIN_CHOMP_FENCE 0x29, daObjWanwanShutter_c).
 */
/* leftovers: the rows of data_ov014_0211476c reach the twelve state members
 * through 8-byte PMF descriptors that stay ROM-supplied data; the table copies
 * them by value. g_profile_WANWAN is not this file's data. The factory
 * constructs the real class, including its two Vector3[7] arrays. The four
 * resource handles keep address names;
 * their definitions drive this file's static initializer.
 */

#include "common.h"
#include "daWanwan_c.h"
#include "daBmb_c.h"
#include "daObjPile_c.h"
#include "daObjWanwanShutter_c.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "Sound.h"
#include "dCc_c.h"
#include "dExtFrameCtrl_c.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"

enum {
    kYoshiEggId = 9,
    kStumpActorId = 0x1b,
    kFenceActorId = 0x29,
    kPlayerActorId = 0xbf,
    kBobOmbActorId = 0xce,
    kRestLift = 0xc8000,   /* 200.0, spawn offset and rest height */
    kChainSlack = 0x50000, /* idle leash */
    kChainMax = 0x64000    /* fully extended lunge */
};

/* This int-returning predicate is what makes the ROM materialize 0/1
   before testing it. */
static inline int IsActorID(dActor_c *actor, int id) {
    return actor->actorID == id;
}

/* dExtSimpleModel_c handles construct through func_02017acc and destroy through
 * func_02017ab4; animation handles construct through SharedFilePtr::Construct
 * and destroy through SharedFilePtr_Destruct_Anim. The manifest aliases those
 * undefined members onto the ROM symbols. filePtr is the BMD or BCA the load
 * just filled in.
 *   02114968  body BMD   sinit 0x9c02, dExtSimpleModel_c::LoadFile, mAnmModel
 *   02114978  link BMD   sinit 0x9c01, dExtSimpleModel_c::LoadFile, mLinkModels
 *   02114980  idle BCA   sinit 0x9c04, dExtFrameCtrl_c::LoadFile
 *   02114970  lunge BCA  sinit 0x9c03, dExtFrameCtrl_c::LoadFile
 */
struct WanwanModelFilePtr : SharedFilePtr {
    WanwanModelFilePtr(u32 fileID);
    ~WanwanModelFilePtr();
};

struct WanwanAnimationFileHandle : SharedFilePtr {
    WanwanAnimationFileHandle(u32 fileID);
    ~WanwanAnimationFileHandle();
};

extern "C" {
extern WanwanModelFilePtr data_ov014_02114968;
extern WanwanModelFilePtr data_ov014_02114978;
extern WanwanAnimationFileHandle data_ov014_02114980;
extern WanwanAnimationFileHandle data_ov014_02114970;
extern int data_ov014_02114700[]; /* collision offset (0, -200, 0) */
extern const Matrix4x3 IDENTITY_MATRIX4X3;
extern dCamera_c *gCamera;  /* CAMERA */

void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
void Vec3_MulScalar(Vector3 *out, const Vector3 *v, int s);
int AngleDiff(int a, int b);
int Math_Function_0203b14c(void *p, int a, int b, int c, int d);
/* local extern: no header this file includes declares it, and it must keep C
 * linkage now that its caller StateEscapeDemoMain is a member. Nonzero once
 * the position has reached the target. */
s32 Vec3_ApproachHorz(Vector3 *out, Vector3 *target, s32 maxStep);
}
short Vec3_VertAngle(const Vector3 *a, const Vector3 *b);

namespace cstd {
s16 atan2(s32, s32);
}

#define SetChompAnim(self, bca) \
    (self)->mAnmModel.SetAnim((BCA_File *)(bca).filePtr, 0, 0x1000, 0)

daWanwan_c *daWanwan_c_classInit() {
    return new daWanwan_c();
}

int daWanwan_c::InitResources() {
    BMD_File *f = dExtSimpleModel_c::LoadFile(data_ov014_02114968);
    mAnmModel.SetFile(f, 1, 1);
    dExtSimpleModel_c::LoadFile(data_ov014_02114978);
    dExtFrameCtrl_c::LoadFile(data_ov014_02114980);
    dExtFrameCtrl_c::LoadFile(data_ov014_02114970);

    for (int i = 0; i < 7; i++)
        mLinkModels[i].SetFile((BMD_File *)data_ov014_02114978.filePtr, 1, 1);

    mShadowModel.InitCylinder();
    for (int i = 0; i < 7; i++)
        mLinkShadows[i].InitCylinder();

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;

    {
        int v[3];
        v[0] = data_ov014_02114700[0];
        v[1] = data_ov014_02114700[1];
        v[2] = data_ov014_02114700[2];
        mdCcAcPos_c.Init(this, *((Vector3 *)v), 0x96000, 0x12c000, 0x200004, 0x26ff0);
    }

    SetState(1);

    {
        /* dst is the actor address advanced by one Vector3 per link, so
           dst->mLinkPos[0] is link cnt's position. A pointer that already
           points at the array is two instructions longer. */
        int cnt = 0;
        daWanwan_c *dst = this;
        do {
            dst->mLinkPos[0].x = mPos.x;
            cnt = cnt + 1;
            dst->mLinkPos[0].y = mPos.y;
            dst->mLinkPos[0].z = mPos.z;
            dst = (daWanwan_c *)((char *)dst + sizeof(Vector3));
        } while (cnt < 7);
    }

    mSpawnPos.x = mPos.x;
    mSpawnPos.y = mPos.y;
    mSpawnPos.z = mPos.z;

    dActor_c *spawned = dActor_c::Spawn(kStumpActorId, 0x11, mPos, 0, mAreaId, -1);
    mStumpUniqueID = spawned->uniqueID;
    ((daObjPile_c *)spawned)->mBusy = 1;
    mFenceUniqueID = 0;

    mPos.x += kRestLift;
    mPos.y += kRestLift;
    mPos.z += kRestLift;

    mChainExtension = kChainSlack;
    mScale.x = 0x1000;
    mScale.y = 0x1000;
    mScale.z = 0x1000;
    return 1;
}

int daWanwan_c::Behavior() {
    mIsOnGround = 0;
    {
        int rest = mSpawnPos.y + kRestLift;
        if (mPos.y <= rest) {
            mPos.y = rest;
            if (mWasOnGround == 0)
                PlayLandingEffects();
            mIsOnGround = 1;
        }
    }
    mWasOnGround = mIsOnGround;
    if (mFenceUniqueID == 0) {
        dActor_c *fence = dActor_c::FindWithActorID(kFenceActorId, 0);
        mFenceUniqueID = fence->uniqueID;
    }
    UpdateState();
    UpdatePos(&mdCcAcPos_c);
    HandleCylinderHit();
    if (mChainBroken == 0)
        RestrictToChain();
    UpdateChainSegments();
    if (mChainBroken == 0) {
        CapChainToPivot();
        PinChainToHead();
    }
    UpdateModelTransform();
    {
        int v[3];
        v[0] = data_ov014_02114700[0];
        v[1] = data_ov014_02114700[1];
        v[2] = data_ov014_02114700[2];
        mdCcAcPos_c.SetPosRelativeToActor(*(Vector3 *)v);
    }
    mdCcAcPos_c.Clear();
    if (ClosestPlayer()->mIsVanish == 0)
        mdCcAcPos_c.Update();
    return 1;
}

int daWanwan_c::Render() {
    mAnmModel.Render(&mScale);

    int j = 0;
    dExtSimpleModel_c *p = mLinkModels;
    for (;;) {
        p->Render(0);
        j++;
        p = p + 1;
        if (j >= 7)
            break;
    }
    return 1;
}

int daWanwan_c::CleanupResources() {
    data_ov014_02114968.Release();
    data_ov014_02114978.Release();
    data_ov014_02114980.Release();
    data_ov014_02114970.Release();
    return 1;
}

/* Place the head and each link, then drop a cylinder shadow under them. */
void daWanwan_c::UpdateModelTransform() {
    /* Each matrix's translation is position >> 3.
       m and sm walk mLinkModels and mLinkShadows. e and o are the actor
       address advanced by one Vector3 and one dExtSimpleModel_c per link, so
       e->mLinkPos[0] is link i's position and o->mLinkModels[0] is the model
       m points at. Storing the translation through m, or walking mLinkPos
       with a Vector3 pointer, changes the emitted loop. */
    Matrix4x3 tmp;
    int i;
    int t;
    dExtSimpleModel_c *m;
    daWanwan_c *e;
    daWanwan_c *o;
    dExtShadowModel_c *sm;
    Matrix4x3_FromRotationXYZExt(&mAnmModel.mat4x3, mAngle.x, mAngle.y, mAngle.z);
    mAnmModel.mat4x3.t.x = mPos.x >> 3;
    mAnmModel.mat4x3.t.y = mPos.y >> 3;
    mAnmModel.mat4x3.t.z = mPos.z >> 3;
    t = mPos.y - mSpawnPos.y;
    if (t <= 0x1000)
        t = 0x1000;
    DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, 0x15e000 - FX_MUL(t, 0x180), t + 0x28000,
                        0xf);
    tmp = IDENTITY_MATRIX4X3;
    sm = mLinkShadows;
    i = 0;
    m = mLinkModels;
    e = this;
    o = this;
    for (; i < 7; i++) {
        m->mat4x3 = tmp;
        o->mLinkModels[0].mat4x3.t.x = e->mLinkPos[0].x >> 3;
        o->mLinkModels[0].mat4x3.t.y = e->mLinkPos[0].y >> 3;
        o->mLinkModels[0].mat4x3.t.z = e->mLinkPos[0].z >> 3;
        t = e->mLinkPos[0].y - mSpawnPos.y;
        if (t <= 0x1000)
            t = 0x1000;
        DropShadowRadHeight(*sm, m->mat4x3, 0x78000 - FX_MUL(t, 0x180), t + 0x28000, 0xf);
        m++;
        e = (daWanwan_c *)((char *)e + sizeof(Vector3));
        o = (daWanwan_c *)((char *)o + sizeof(dExtSimpleModel_c));
        sm++;
    }
}

/* Forward chain: head at the actor, then links 1..6, carrying mLinkDelta. */
void daWanwan_c::UpdateChainSegments() {
    int yoff;
    Vector3 dvec;
    Vector3 head;
    Vector3 rotated;
    Vector3 saved;
    Vector3 tmp;
    Vector3 sum;
    Vector3 diff;
    Vector3 mul;
    int scale;
    Vector3 *seg;
    Vector3 *dst;
    int *bound;
    int state;
    short angY;
    short angX;
    Vector3 *prev;
    int i;

    head.x = 0;
    head.y = 0;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;
    head.z = -kRestLift;

    /* setup order is load-bearing: seg before state so add/cmp schedule matches */
    seg = &mLinkPos[1];
    dst = &mLinkDelta[1];
    state = mState;
    bound = mLinkFloorY;
    scale = 0xb68;
    yoff = -0x5000;

    if (state == 2 || state == 4) {
        if (mChainBroken == 0) {
            scale += 0x7800;
            yoff = 0;
        } else {
            scale += 0x190;
        }
    }

    Matrix4x3_FromRotationXYZExt(&data_020a0e68, mAngle.x, mAngle.y, mAngle.z);
    MTX_MultVec43(&head, &data_020a0e68, &rotated);
    Vec3_Add(&tmp, &mPos, &rotated);
    saved.x = tmp.x;
    saved.y = tmp.y;
    saved.z = tmp.z;
    mLinkPos[0].x = tmp.x;
    mLinkPos[0].y = saved.y;
    mLinkPos[0].z = saved.z;

    head.z = mChainExtension;
    head.x = 0;
    head.y = 0;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;
    i = 1;

    for (; i < 7; i++, bound++) {
        if (i == 0)
            prev = &saved;
        else
            prev = seg - 1;

        dvec.x = (seg->x - prev->x) + dst->x;
        dvec.z = (seg->z - prev->z) + dst->z;
        {
            int t = (seg->y + dst->y) + yoff;
            if (t <= *bound)
                t = *bound;
            dvec.y = t - prev->y;
        }

        angY = cstd::atan2(dvec.x, dvec.z);
        angX = (short)(-cstd::atan2(dvec.y, Vec3_HorzLen(&dvec)));
        Matrix4x3_FromRotationY(&data_020a0e68, angY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, angX);
        MTX_MultVec43(&head, &data_020a0e68, &rotated);

        dst->x = seg->x;
        dst->y = seg->y;
        dst->z = seg->z;
        Vec3_Add(&sum, prev, &rotated);
        seg->x = sum.x;
        seg->y = sum.y;
        seg->z = sum.z;
        Vec3_Sub(&diff, seg, dst);
        Vec3_MulScalar(&mul, &diff, scale);
        dst->x = mul.x;
        dst->y = mul.y;
        dst->z = mul.z;

        if (*bound <= mSpawnPos.y)
            *bound = mSpawnPos.y + 0x28000;

        seg++;
        dst++;
    }
}

/* Backward chain, from the stump end (link 6) toward the head: each link is
 * pulled to mChainExtension from the one behind it, link 6 from a point
 * 0x1e000 above the stump. */
void daWanwan_c::CapChainToPivot() {
    Vector3 tmp;
    Vector3 head;
    Vector3 rotated;
    Vector3 sum;
    Vector3 prevpos;
    Vector3 diff;
    Vector3 delta;
    Vector3 scaled;
    Vector3 mul;
    short angY;
    Vector3 *target;
    Vector3 *cur;
    u32 cursor;
    short angX;
    int i;
    int z;

    /* This setup runs twice. Without the first copy the body is 28 bytes
       shorter. */
    z = mChainExtension;
    head.x = 0;
    head.y = 0;
    head.z = z;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;

    prevpos.x = mSpawnPos.x;
    {
        int py = mSpawnPos.y;
        prevpos.y = py;
        prevpos.z = mSpawnPos.z;
        prevpos.y = py + 0x1e000;
    }

    z = mChainExtension;
    head.x = 0;
    head.y = 0;
    head.z = z;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;

    cursor = (u32)&mLinkPos[6];
    i = 6;
    do {
        cur = (Vector3 *)cursor;
        target = (i == 6) ? &prevpos : cur + 1;
        Vec3_Sub(&diff, cur, target);
        tmp.x = diff.x;
        tmp.y = diff.y;
        tmp.z = diff.z;
        angY = cstd::atan2(tmp.x, tmp.z);
        angX = (short)(-cstd::atan2(tmp.y, Vec3_HorzLen(&tmp)));
        Matrix4x3_FromRotationY(&data_020a0e68, angY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, angX);
        MTX_MultVec43(&head, &data_020a0e68, &rotated);
        sum.x = cur->x;
        sum.y = cur->y;
        sum.z = cur->z;
        Vec3_Add(&delta, target, &rotated);
        cur->x = delta.x;
        cur->y = delta.y;
        cur->z = delta.z;
        Vec3_Sub(&scaled, cur, &sum);
        Vec3_MulScalar(&mul, &scaled, 0xb68);
        sum.x = mul.x;
        sum.y = mul.y;
        sum.z = mul.z;
        --i;
        /* Advance a target address; convert only valid elements above. */
        cursor -= sizeof(*cur);
    } while (i >= 0);
}

/* Blend the actor position into the chain. Integer (7-i)/7 is 1 only for i == 0. */
void daWanwan_c::PinChainToHead() {
    Vector3 *p;
    int i;
    Vector3 diff;
    Vector3 scaled;

    Vec3_Sub(&diff, &mPos, &mLinkPos[0]);
    p = &mLinkPos[0];
    i = 0;
    do {
        Vec3_MulScalar(&scaled, &diff, (7 - i) / 7);
        VEC_Add(p, &scaled, p);
        i++;
        p++;
    } while (i < 7);
}

/* Hit response. Egg (actor 9, hit 0x2000) pops the scale into state 0.
 * Explosion (0x4000) and a Bob-omb (0xce) go to state 4. A mega hit from
 * the player (0xbf, hit 0x10) starts the bite (state 5) and hurts when facing. */
void daWanwan_c::HandleCylinderHit() {
    dActor_c *other;
    unsigned id;
    int ang;

    if ((unsigned)(mState - 3) <= 1)
        return;
    id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        return;
    other = dActor_c::FindWithID(id);
    if (other == 0)
        return;

    if (IsActorID(other, kYoshiEggId)) {
        if (mdCcAcPos_c.hitFlags & 0x2000) {
            mScale.x = 0x2000;
            mScale.y = mScale.x;
            mScale.z = mScale.y;
            SetState(0);
            return;
        }
    }

    if (mdCcAcPos_c.hitFlags & 0x4000) {
        mScale.x = 0x2000;
        mScale.y = mScale.x;
        mScale.z = mScale.y;
        SetState(4);
        return;
    }

    if (IsActorID(other, kPlayerActorId)) {
        ang = Vec3_HorzAngle(&mPos, &other->mPos);
        if (mdCcAcPos_c.hitFlags & 0x10) {
            SetState(5);
            mPrevAngle.y = other->mAngle.y;
        }
        if (AngleDiff(ang, mAngle.y) < 0x4000) {
            Vector3 v;
            v.x = mPos.x;
            v.y = mPos.y;
            v.z = mPos.z;
            ((daPly_c *)other)->Hurt(v, 3, 0xc000, 1, 0, 1);
        }
    }

    if (mState == 4)
        return;
    if (IsActorID(other, kBobOmbActorId)) {
        ((daBmb_c *)other)->func_ov102_0214ae1c();
        SetState(4);
    }
}

/* Pull the chomp back inside chainExtension * 7 + 200 of the spawn point.
 * A fully extended lunge (state 2, extension 0x64000) also kills both speeds. */
void daWanwan_c::RestrictToChain() {
    Vector3 v;
    Vector3 out;
    int len;
    int lim;
    Vec3_Sub(&v, &mPos, &mSpawnPos);
    len = VEC_Mag(&v);
    lim = mChainExtension * 7 + kRestLift;
    if (len <= lim)
        return;
    v.x = FX_MUL(v.x, FX_Div(lim, len));
    v.y = FX_MUL(v.y, FX_Div(lim, len));
    v.z = FX_MUL(v.z, FX_Div(lim, len));
    Vec3_Add(&out, &mSpawnPos, &v);
    mPos.x = out.x;
    mPos.y = out.y;
    mPos.z = out.z;
    if (mState != 2)
        return;
    if (mChainExtension == kChainMax) {
        mHorzSpeed = 0;
        mSpeed.y = 0;
    }
}

/* Landing one-shot. Sound 0x39 at camera-space position, then the dust. */
void daWanwan_c::PlayLandingEffects() {
    func_0201267c(0x39, &mCamSpacePos);
    HugeLandingDust(true);
}

/* Start the release cutscene when the stump's mStepsLeft is 0 and the player
 * accepts the no-control lock. */
int daWanwan_c::TryStartReleaseCutscene() {
    daObjPile_c *stump = (daObjPile_c *)dActor_c::FindWithID((unsigned)mStumpUniqueID);
    if (stump->mStepsLeft == 0 && ClosestPlayer()->SetNoControlState(4, -1, 0) != 0) {
        SetState(3);
        mChainBroken = 1;
        return 1;
    }
    return 0;
}

/* CodeWarrior's pointer-to-member is 8 bytes for daWanwan_c, a complete
 * single-inheritance class here, so each row of the state table
 * data_ov014_0211476c is 20 bytes: enter PMF at +0, update PMF at +8, name
 * pointer at +0x10. The table is defined at the bottom of this file; the
 * initializer copies each row's enter/update pair by value from twelve 8-byte
 * PMF descriptors in .data at 021146a0..021146f8 (the name pointers are the
 * only static part). Measured from those descriptors and the name pointers;
 * each Init/Main pair is named for its row's name string:
 *   0  StateWaitInit         StateWaitMain         scale back to 1.0      WAIT
 *   1  StateWalkInit         StateWalkMain         idle, face the player  WALK
 *   2  StateAttackInit       StateAttackMain       lunge                  ATTACK
 *   3  StateEscapeDemoInit   StateEscapeDemoMain   break chain and fence  ESCAPE_DEMO
 *   4  StateBombDamageInit   StateBombDamageMain   knocked                BOMB_DAMAGE
 *   5  StateGiantDamageInit  StateGiantDamageMain  bite                   GIANT_DAMAGE
 * SetState runs the PMF at +0 (enter). UpdateState runs the PMF at +8 (update).
 */
typedef void (daWanwan_c::*WanwanPmf)();

/* The descriptor objects the initializer copies into the table. */
extern "C" WanwanPmf data_ov014_021146e0;
extern "C" WanwanPmf data_ov014_021146b8;
extern "C" WanwanPmf data_ov014_021146f0;
extern "C" WanwanPmf data_ov014_021146c0;
extern "C" WanwanPmf data_ov014_021146a8;
extern "C" WanwanPmf data_ov014_021146a0;
extern "C" WanwanPmf data_ov014_021146f8;
extern "C" WanwanPmf data_ov014_021146b0;
extern "C" WanwanPmf data_ov014_021146d8;
extern "C" WanwanPmf data_ov014_021146c8;
extern "C" WanwanPmf data_ov014_021146e8;
extern "C" WanwanPmf data_ov014_021146d0;

/* The state-name strings the table's name pointers reference. */
extern "C" char data_ov014_02114690[]; /* WAIT */
extern "C" char data_ov014_02114688[]; /* WALK */
extern "C" char data_ov014_02114698[]; /* ATTACK */
extern "C" char data_ov014_0211470c[]; /* ESCAPE_DEMO */
extern "C" char data_ov014_02114724[]; /* BOMB_DAMAGE */
extern "C" char data_ov014_02114730[]; /* GIANT_DAMAGE */

struct WanwanStateRow {
    WanwanPmf enter;
    WanwanPmf update;
    const char *name;
};
extern "C" WanwanStateRow data_ov014_0211476c[6];

void daWanwan_c::UpdateState() {
    (this->*data_ov014_0211476c[mState].update)();
}

void daWanwan_c::SetState(int i) {
    mState = i;
    (this->*data_ov014_0211476c[mState].enter)();
}

/* State 0 enter: stop, hold for 0x78 frames, play the idle BCA. */
void daWanwan_c::StateWaitInit() {
    mSpeed.y = 0;
    mHorzSpeed = 0;
    mActionTimer = 120;
    SetChompAnim(this, data_ov014_02114980);
}

/* State 0 update: scale back to 1.0, then idle. */
void daWanwan_c::StateWaitMain() {
    mSpeed.y = 0;
    ApproachLinear(mScale.x, 0x1000, 0x500);
    mScale.z = mScale.x;
    mScale.y = mScale.z;
    if (DecIfAbove0_Short(&mActionTimer) != 0)
        return;
    SetState(1);
}

/* State 1 enter: gravity back on, idle BCA, 0x78-frame pause. */
void daWanwan_c::StateWalkInit() {
    mSpeed.y = 0;
    mVertAccel = -0x2000;
    mActionTimer = 120;
    SetChompAnim(this, data_ov014_02114980);
}

/* State 1 update. Tries the release, turns toward mTargetAngY, and on the
 * ground lunges (state 2) when the player is close and in front. */
void daWanwan_c::StateWalkMain() {
    if (TryStartReleaseCutscene())
        return;
    ApproachAngle(&mAngle.x, 0, 4, 0x200, 0x80);
    DecIfAbove0_Short(&mActionTimer);
    ApproachLinear(mChainExtension, kChainSlack, 0x1000);
    ApproachLinear(mAngle.y, mTargetAngY, 0x190);
    ApproachLinear(mHorzSpeed, 0, 0x400);
    if (mIsOnGround) {
        int d = DistToCPlayer();
        mTargetAngY = HorzAngleToCPlayer();
        mPrevAngle.y = mAngle.y;
        mHorzSpeed = 0xa000;
        mSpeed.y = 0x14000;
        unk_600 = 0;
        if (d < 0x500000 && AngleDiff(mTargetAngY, mAngle.y) < 0x800 && mActionTimer == 0) {
            SetState(2);
        }
    }
    static_cast<dExtFrameCtrl_c &>(mAnmModel).Advance();
}

/* State 2 enter: sound 0x3a, lunge BCA, launch along the vertical angle to
 * the player (raised 0x50000). The third argument of the sound call is real
 * in this body; the two-argument definition ignores it. */
void daWanwan_c::StateAttackInit() {
    Vector3 tgt;
    daPly_c *player;
    int a;
    int v;

    mVertAccel = 0;
    func_0201267c(0x3a, &mCamSpacePos);
    SetChompAnim(this, data_ov014_02114970);
    player = ClosestPlayer();

    /* ROM load order: y, z, x — then y+0x50000, store x, setup call, store y/z */
    {
        int y = player->mPos.y;
        int z = player->mPos.z;
        int x = player->mPos.x;
        tgt.x = x;
        tgt.y = y + 0x50000;
        tgt.z = z;
    }

    a = (unsigned short)Vec3_VertAngle(&mPos, &tgt);
    mSpeed.y = -FX_MUL(FX_SinIdx(a >> 4), 0x8c000);
    v = mSpeed.y;
    if (v < 0x5000)
        v = 0x5000;
    else if (v > 0x2d000)
        v = 0x2d000;
    mSpeed.y = v;
    mHorzSpeed = FX_MUL(FX_CosIdx(a >> 4), 0x8c000);
    mActionTimer = 60;
}

/* State 2 update: stretch the leash to kChainMax, then back to idle. */
int daWanwan_c::StateAttackMain() {
    int r = TryStartReleaseCutscene();
    if (r)
        return r;
    if (!Math_Function_0203b14c(&mChainExtension, kChainMax, 0x800, 0x10000, 0x800) &&
        !DecIfAbove0_Short(&mActionTimer))
        SetState(1);
    static_cast<dExtFrameCtrl_c &>(mAnmModel).Advance();
}

/* State 3 enter: idle BCA, snap back to the rest height, clear clip flags. */
void daWanwan_c::StateEscapeDemoInit() {
    SetChompAnim(this, data_ov014_02114980);
    mSpeed.y = 0;
    mHorzSpeed = 0;
    mVertAccel = -0x2000;
    mReleaseStep = 0;
    unk_600 = 0;
    mActionTimer = 60;
    mFlags &= ~3;
    mPos.y = mSpawnPos.y + kRestLift;
}

/* State 3 update: the fence-break cutscene. mReleaseStep is the phase. */
void daWanwan_c::StateEscapeDemoMain() {
    daObjWanwanShutter_c *fence;
    Vector3 partnerPos;
    s16 angleToPlayer;
    s16 angleToAnchor;
    dCamera_c *camera;

    Sound::PlaySecretSound(this, &mSecretSound);
    fence = (daObjWanwanShutter_c *)dActor_c::FindWithID((unsigned)mFenceUniqueID);
    {
        s32 *src = &fence->mPos.x;
        partnerPos.x = src[0];
        partnerPos.y = src[1];
        partnerPos.z = src[2];
        ApproachAngle(&mAngle.x, 0, 4, 0x200, 0x80);
    }
    camera = gCamera;
    angleToPlayer = HorzAngleToCPlayer();
    angleToAnchor = Vec3_HorzAngle(&mPos, &mSpawnPos);
    /* Steps 0-6 make the chomp the camera's mFocusActor. */
    switch (mReleaseStep) {
    case 0:
        camera->mFocusActor = this;
        if (ApproachLinear(mAngle.y, angleToPlayer, 0x320) != 0 &&
            DecIfAbove0_Short(&mActionTimer) == 0)
            mReleaseStep++;
        mPrevAngle.y = mAngle.y;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        camera->mFocusActor = this;
        ApproachLinear(mHorzSpeed, 0, 0x400);
        if (mIsOnGround != 0) {
            mPrevAngle.y = (s16)(angleToAnchor + 0x2000);
            mAngle.y = mPrevAngle.y;
            mSpeed.y = 0x32000;
            mHorzSpeed = 0x1e000;
            mReleaseStep++;
        }
        break;
    case 5:
        camera->mFocusActor = this;
        ApproachLinear(mHorzSpeed, 0, 0x400);
        if (mIsOnGround != 0) {
            mPrevAngle.y = angleToAnchor;
            mAngle.y = mPrevAngle.y;
            mSpeed.y = 0x32000;
            mHorzSpeed = 0x1e000;
            mReleaseStep++;
            if (fence->mDisabled != 0)
                mReleaseStep = 7;
        }
        break;
    case 6: {
        camera->mFocusActor = this;
        ApproachLinear(mReleaseSpeed, 0x1000, 0x400);
        if (mIsOnGround != 0) {
            SetChompAnim(this, data_ov014_02114970);
            mAngle.y = Vec3_HorzAngle(&mPos, &partnerPos);
            mPrevAngle.y = mAngle.y;
            mSpeed.y = 0x14000;
            mReleaseSpeed = kChainMax;
        }
        {
            Vector3 target = partnerPos;
            target.x += FX_MUL(FX_SinIdx((u16)fence->mAngle.y >> 4), 0x96000);
            target.z += FX_MUL(FX_CosIdx((u16)fence->mAngle.y >> 4), 0x96000);
            if (Vec3_ApproachHorz(&mPos, &target, mReleaseSpeed) != 0) {
                mReleaseStep++;
                mPrevAngle.y = (s16)(Vec3_HorzAngle(&mPos, &partnerPos) + 0x8000);
                mHorzSpeed = 0x28000;
                mSpeed.y = 0xa000;
                fence->Break();
            }
        }
        break;
    }
    case 7:
        if (mIsOnGround != 0) {
            SetChompAnim(this, data_ov014_02114980);
            mPrevAngle.y = Vec3_HorzAngle(&mPos, &partnerPos);
            mActionTimer = 60;
            mAngle.y = mPrevAngle.y;
            if (fence->mDisabled != 0) {
                mHorzSpeed = 0x28000;
                mSpeed.y = 0x5a000;
            } else {
                mHorzSpeed = 0x1e000;
                mSpeed.y = kChainSlack;
            }
            camera->SetFlag_3();
            mReleaseStep++;
        }
        break;
    case 8:
        if (DecIfAbove0_Short(&mActionTimer) == 0) {
            camera->mFlags &= ~8u;
            if (ClosestPlayer()->Unk_020ca150(4) != 0) {
                mReleaseStep++;
                mActionTimer = 60;
            }
        } else {
            camera->SetLookAt(mPos);
        }
        break;
    case 9:
        if (DecIfAbove0_Short(&mActionTimer) == 0)
            MarkForDestruction();
        break;
    }
    static_cast<dExtFrameCtrl_c &>(mAnmModel).Advance();
}

/* State 4 enter: sound 0x3a and a hard upward speed. */
void daWanwan_c::StateBombDamageInit() {
    func_0201267c(0x3a, &mCamSpacePos);
    mSpeed.y = 0x12c000;
    mVertAccel = 0;
}

/* State 4 update. The early exits fall out of the bottom: the ROM leaves r0
 * alone on those paths, and a valueless return is not available to the host. */
int daWanwan_c::StateBombDamageMain() {
    ApproachLinear(mScale.x, 0x1000, 0x500);
    mScale.z = mScale.x;
    mScale.y = mScale.z;
    ApproachAngle(&mAngle.x, -0x4000, 4, 0x1000, 0x400);
    if (Math_Function_0203b14c(&mChainExtension, kChainMax, 0x800, 0x10000, 0x800) == 0) {
        if (DecIfAbove0_Short(&mActionTimer) == 0)
            SetState(1);
    }
}

/* State 5 enter: face mPrevAngle.y and launch. */
void daWanwan_c::StateGiantDamageInit() {
    mAngle.y = mPrevAngle.y;
    mActionTimer = 60;
    mHorzSpeed = kChainMax;
    mSpeed.y = kRestLift;
    mVertAccel = -0xa000;
}

/* State 5 update: back to idle when the timer ends. On the ground the
 * vertical speed copies the horizontal one, which then brakes. */
void daWanwan_c::StateGiantDamageMain() {
    if (DecIfAbove0_Short(&mActionTimer) == 0)
        SetState(1);
    if (mIsOnGround != 0)
        mSpeed.y = mHorzSpeed;
    ApproachLinear(mHorzSpeed, 0, 0x2000);
}

/* Both variants come from the inline destructor in daWanwan_c.h.
 * That emits D1 then D0 and no D2, which is the ROM order. */

/* Order is the retail initializer: model 0x9c02, model 0x9c01, anim 0x9c04,
 * anim 0x9c03, then the six state rows. The model constructors and both
 * destructor families are the ROM veneers the manifest aliases; mwcc emits
 * __sinit_d_a_wanwan.cpp from these definitions. The table's enter/update
 * fields copy from the ROM PMF descriptors by value; the name pointers are
 * the static part. */
WanwanModelFilePtr data_ov014_02114968(0x9c02);
WanwanModelFilePtr data_ov014_02114978(0x9c01);
WanwanAnimationFileHandle data_ov014_02114980(0x9c04);
WanwanAnimationFileHandle data_ov014_02114970(0x9c03);

extern "C" WanwanStateRow data_ov014_0211476c[6] = {
    {data_ov014_021146e0, data_ov014_021146b8, data_ov014_02114690},
    {data_ov014_021146f0, data_ov014_021146c0, data_ov014_02114688},
    {data_ov014_021146a8, data_ov014_021146a0, data_ov014_02114698},
    {data_ov014_021146f8, data_ov014_021146b0, data_ov014_0211470c},
    {data_ov014_021146d8, data_ov014_021146c8, data_ov014_02114724},
    {data_ov014_021146e8, data_ov014_021146d0, data_ov014_02114730},
};

/* Names and the collision offset. Pointer-to-member descriptors stay ROM
   copies: mwcc puts `&Method` in .bss and copies it from an @ temporary. */
char data_ov014_02114688[8] = "WALK";
char data_ov014_02114690[8] = "WAIT";
char data_ov014_02114698[8] = "ATTACK";
char data_ov014_0211470c[12] = "ESCAPE_DEMO";
char data_ov014_02114724[12] = "BOMB_DAMAGE";
char data_ov014_02114730[16] = "GIANT_DAMAGE";
int data_ov014_02114700[3] = {0, 0xfff38000, 0};
