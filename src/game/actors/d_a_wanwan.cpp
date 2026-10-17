//cpp
/**
 * daWanwan_c -- Bob-omb Battlefield Chain Chomp. Seven links, the stump it
 * spawns (STUMP 0x1b, daObjPile_c) and the fence it finds
 * (CHAIN_CHOMP_FENCE 0x29, daObjWanwanShutter_c).
 *
 * common.h stays first so the shadow matrix copy stays twelve words.
 */
/* leftovers: the chain helpers and the twelve state handlers stay extern "C"
 * free functions under their func_ov014_* address names; the rows of
 * data_ov014_0211476c reach the handlers through 8-byte PMF descriptors.
 * g_profile_WANWAN is not this file's data. ModelAnim::SetAnim,
 * dCcAcPos_c::Init, DropShadowRadHeight, Player::Hurt and cstd::atan2
 * stay mangled: this file calls them, and each takes Fix12<int> by
 * value. The factory constructs the real class, including its two
 * Vector3[7] arrays. The four resource handles keep address names;
 * their definitions drive this file's static initializer.
 */

#include "common.h"
#include "daWanwan_c.h"
#include "daObjPile_c.h"
#include "daObjWanwanShutter_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "dCamera_c.h"
#include "Sound.h"
#include "dCc_c.h"
#include "dExtFrameCtrl_c.h"
#include "nitro/fx/fx.h"

enum {
    kYoshiEggId = 9,
    kStumpActorId = 0x1b,
    kFenceActorId = 0x29,
    kPlayerActorId = 0xbf,
    kBobOmbActorId = 0xce,
    kRestLift = 0xc8000,       /* 200.0, spawn offset and rest height */
    kChainSlack = 0x50000,     /* idle leash */
    kChainMax = 0x64000        /* fully extended lunge */
};

/* Model handles construct through func_02017acc and destroy through
 * func_02017ab4; animation handles construct through SharedFilePtr::Construct
 * and destroy through SharedFilePtr_Destruct_Anim. The manifest aliases those
 * undefined members onto the ROM symbols. words[1] is the BMD or BCA the load
 * just filled in; SharedFilePtr itself has no fields.
 *   02114968  body BMD   sinit 0x9c02, Model::LoadFile, mModelAnim
 *   02114978  link BMD   sinit 0x9c01, Model::LoadFile, mLinkModels
 *   02114980  idle BCA   sinit 0x9c04, dExtFrameCtrl_c::LoadFile
 *   02114970  lunge BCA  sinit 0x9c03, dExtFrameCtrl_c::LoadFile
 */
struct WanwanModelFilePtr : SharedFilePtr {
    u32 words[2];

    WanwanModelFilePtr(u32 fileID);
    ~WanwanModelFilePtr();
};

struct WanwanAnimationFileHandle : SharedFilePtr {
    u32 words[2];

    WanwanAnimationFileHandle(u32 fileID);
    ~WanwanAnimationFileHandle();
};

extern "C" {
extern WanwanModelFilePtr data_ov014_02114968;
extern WanwanModelFilePtr data_ov014_02114978;
extern WanwanAnimationFileHandle data_ov014_02114980;
extern WanwanAnimationFileHandle data_ov014_02114970;
extern int data_ov014_02114700[]; /* collision offset (0, -200, 0) */
extern Matrix4x3 data_020a0e68;     /* MATRIX_SCRATCH_PAPER */
extern const Matrix4x3 IDENTITY_MATRIX4X3;
extern s16 data_02082214[];         /* SINE_TABLE: sin, cos pairs */
extern void *data_0209f318;         /* CAMERA */

void func_ov014_02111fb8(daWanwan_c *c);
void func_ov014_02112114(daWanwan_c *c);
void func_ov014_02111fe0(daWanwan_c *c);
void func_ov014_0211250c(daWanwan_c *c);
void func_ov014_0211236c(daWanwan_c *c);
void func_ov014_021122dc(daWanwan_c *c);
void func_ov014_02112788(daWanwan_c *c);
void func_ov014_02112ea8(void *fence);
void func_ov102_0214ae1c(void *bomb);

/* Scalar stand-ins. The header methods take Fix12<int> by value. */
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *pos, int radius, int height,
    unsigned flags, unsigned vuln);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
    dActor_c *self, dExtShadowModel_c *shadow, Matrix4x3 *mtx, int radius, int depth,
    unsigned opacity);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, void *bca, int flags, int speed, unsigned start);
/* local extern: Fix12<int> by value; this scalar bridge follows the definition. */
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    Player *player, void *pos, unsigned kind, int power, u8 a, u8 b, u8 c);
short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);

void MulVec3Mat4x3(void *v, void *m, void *dst);
void Vec3_Add(void *out, void *a, void *b);
void Vec3_Sub(void *out, void *a, void *b);
void Vec3_MulScalar(void *out, void *v, int s);
void AddVec3(void *a, void *b, void *c);
int Vec3_HorzLen(void *v);
int LenVec3(Vector3 *v);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
short Vec3_VertAngle(const void *a, const void *b);
void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_FromRotationY(void *m, int ang);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
int AngleDiff(int a, int b);
int func_0201267c(int id, void *pos);
int ApproachAngle(short *angles, int target, int a, int b, int c);
unsigned short DecIfAbove0_Short(unsigned short *p);
int Math_Function_0203b14c(void *p, int a, int b, int c, int d);
}

int ApproachLinear(int &value, int target, int step);
bool ApproachLinear(short &value, short target, short step);

namespace cstd {
int fdiv(int a, int b);
}

namespace call3_267c {
extern "C" int func_0201267c(int id, void *pos, int unused);
}

/* SetAnim takes Fix12<int> by value. The scalar extern stays. */
#define SetChompAnim(self, bca) \
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj( \
        &(self)->mModelAnim, (BCA_File *)(bca).words[1], 0, 0x1000, 0)

// @symbol daWanwan_c_classInit
extern "C" daWanwan_c *daWanwan_c_classInit()
{
    return new daWanwan_c();
}

// @symbol _ZN10daWanwan_c13InitResourcesEv
int daWanwan_c::InitResources()
{
    void *f = Model::LoadFile(data_ov014_02114968);
    mModelAnim.SetFile((BMD_File *)f, 1, 1);
    Model::LoadFile(data_ov014_02114978);
    dExtFrameCtrl_c::LoadFile(data_ov014_02114980);
    dExtFrameCtrl_c::LoadFile(data_ov014_02114970);

    {
        int i = 0;
        Model *p = mLinkModels;
        do {
            p->SetFile((BMD_File *)data_ov014_02114978.words[1], 1, 1);
            i = i + 1;
            p++;
        } while (i < 7);
    }

    mShadowModel.InitCylinder();
    {
        int si = 0;
        dExtShadowModel_c *sp = mLinkShadows;
        do {
            sp->InitCylinder();
            si = si + 1;
            sp++;
        } while (si < 7);
    }

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;

    {
        int v[3];
        v[0] = data_ov014_02114700[0];
        v[1] = data_ov014_02114700[1];
        v[2] = data_ov014_02114700[2];
        /* dCcAcPos_c::Init takes Fix12<int> by value. */
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            &mdCcAcPos_c, this, (Vector3 *)v, 0x96000, 0x12c000, 0x200004, 0x26ff0);
    }

    func_ov014_02111ebc(1);

    {
        /* dst is the actor address advanced by one Vector3 per link, so
           dst->mLinkPos[0] is link cnt's position. A pointer that already
           points at the array is two instructions longer. */
        int cnt = 0;
        daWanwan_c *dst = this;
        do {
            dst->mLinkPos[0].x = mPosX;
            cnt = cnt + 1;
            dst->mLinkPos[0].y = mPosY;
            dst->mLinkPos[0].z = mPosZ;
            dst = (daWanwan_c *)((char *)dst + sizeof(Vector3));
        } while (cnt < 7);
    }

    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;

    dActor_c *spawned = dActor_c::Spawn(kStumpActorId, 0x11, mPos, 0, mAreaId, -1);
    mStumpUniqueID = spawned->uniqueID;
    int one = 1;
    ((daObjPile_c *)spawned)->mBusy = (unsigned char)one;
    mFenceUniqueID = 0;

    mPosX = mPosX + kRestLift;
    mPosY = mPosY + kRestLift;
    mPosZ = mPosZ + kRestLift;

    mChainExtension = kChainSlack;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    return one;
}

// @symbol _ZN10daWanwan_c8BehaviorEv
int daWanwan_c::Behavior()
{
    mIsOnGround = 0;
    {
        int rest = mSpawnPosY + kRestLift;
        if (mPosY <= rest) {
            mPosY = rest;
            if (mWasOnGround == 0)
                func_ov014_02111fb8(this);
            mIsOnGround = 1;
        }
    }
    mWasOnGround = mIsOnGround;
    if (mFenceUniqueID == 0) {
        dActor_c *fence = dActor_c::FindWithActorID(kFenceActorId, 0);
        mFenceUniqueID = fence->uniqueID;
    }
    func_ov014_02111f08();
    UpdatePos(&mdCcAcPos_c);
    func_ov014_02112114(this);
    if (mChainBroken == 0)
        func_ov014_02111fe0(this);
    func_ov014_0211250c(this);
    if (mChainBroken == 0) {
        func_ov014_0211236c(this);
        func_ov014_021122dc(this);
    }
    func_ov014_02112788(this);
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

// @symbol _ZN10daWanwan_c6RenderEv
int daWanwan_c::Render()
{
    mModelAnim.Render(&mScale);

    int j = 0;
    Model *p = mLinkModels;
    for (;;) {
        p->Render(0);
        j++;
        p = p + 1;
        if (j >= 7)
            break;
    }
    return 1;
}

// @symbol _ZN10daWanwan_c16CleanupResourcesEv
int daWanwan_c::CleanupResources()
{
    data_ov014_02114968.Release();
    data_ov014_02114978.Release();
    data_ov014_02114980.Release();
    data_ov014_02114970.Release();
    return 1;
}

/* Place the head and each link, then drop a cylinder shadow under them. */
extern "C" {
// @symbol func_ov014_02112788
void func_ov014_02112788(daWanwan_c *c)
{
    /* Translation is m[9..11] of each matrix: position >> 3.
       m and sm walk mLinkModels and mLinkShadows. e and o are the actor
       address advanced by one Vector3 and one Model per link, so
       e->mLinkPos[0] is link i's position and o->mLinkModels[0] is the model
       m points at. Storing the translation through m, or walking mLinkPos
       with a Vector3 pointer, changes the emitted loop. */
    Matrix4x3 tmp;
    int i;
    int t;
    Model *m;
    daWanwan_c *e;
    daWanwan_c *o;
    dExtShadowModel_c *sm;
    Matrix4x3_FromRotationXYZExt(&c->mModelAnim.mat4x3, c->mAngleX, c->mAngleY, c->mAngleZ);
    c->mModelAnim.mat4x3.m[9] = c->mPosX >> 3;
    c->mModelAnim.mat4x3.m[10] = c->mPosY >> 3;
    c->mModelAnim.mat4x3.m[11] = c->mPosZ >> 3;
    t = c->mPosY - c->mSpawnPosY;
    if (t <= 0x1000)
        t = 0x1000;
    /* DropShadowRadHeight takes Fix12<int> by value. */
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        c, &c->mShadowModel, &c->mModelAnim.mat4x3, 0x15e000 - FX_MUL(t, 0x180), t + 0x28000, 0xf);
    tmp = IDENTITY_MATRIX4X3;
    sm = c->mLinkShadows;
    i = 0;
    m = c->mLinkModels;
    e = c;
    o = c;
    for (; i < 7; i++) {
        m->mat4x3 = tmp;
        o->mLinkModels[0].mat4x3.m[9] = e->mLinkPos[0].x >> 3;
        o->mLinkModels[0].mat4x3.m[10] = e->mLinkPos[0].y >> 3;
        o->mLinkModels[0].mat4x3.m[11] = e->mLinkPos[0].z >> 3;
        t = e->mLinkPos[0].y - c->mSpawnPosY;
        if (t <= 0x1000)
            t = 0x1000;
        _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
            c, sm, &m->mat4x3, 0x78000 - FX_MUL(t, 0x180), t + 0x28000, 0xf);
        m++;
        e = (daWanwan_c *)((char *)e + sizeof(Vector3));
        o = (daWanwan_c *)((char *)o + sizeof(Model));
        sm++;
    }
}
}

/* Forward chain: head at the actor, then links 1..6, carrying mLinkDelta. */
extern "C" {
// @symbol func_ov014_0211250c
void func_ov014_0211250c(daWanwan_c *c)
{
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
    seg = &c->mLinkPos[1];
    dst = &c->mLinkDelta[1];
    state = c->mState;
    bound = c->mLinkFloorY;
    scale = 0xb68;
    yoff = -0x5000;

    if (state == 2 || state == 4) {
        if (c->mChainBroken == 0) {
            scale += 0x7800;
            yoff = 0;
        } else {
            scale += 0x190;
        }
    }

    Matrix4x3_FromRotationXYZExt(
        &data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);
    MulVec3Mat4x3(&head, &data_020a0e68, &rotated);
    Vec3_Add(&tmp, &c->mPosX, &rotated);
    saved.x = tmp.x;
    saved.y = tmp.y;
    saved.z = tmp.z;
    c->mLinkPos[0].x = tmp.x;
    c->mLinkPos[0].y = saved.y;
    c->mLinkPos[0].z = saved.z;

    head.z = c->mChainExtension;
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

        /* cstd::atan2 takes Fix12<int> by value. */
        angY = _ZN4cstd5atan2E5Fix12IiES1_(dvec.x, dvec.z);
        angX = (short)(-_ZN4cstd5atan2E5Fix12IiES1_(dvec.y, Vec3_HorzLen(&dvec)));
        Matrix4x3_FromRotationY(&data_020a0e68, angY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, angX);
        MulVec3Mat4x3(&head, &data_020a0e68, &rotated);

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

        if (*bound <= c->mSpawnPosY)
            *bound = c->mSpawnPosY + 0x28000;

        seg++;
        dst++;
    }
}
}

/* Backward chain, from the stump end (link 6) toward the head. */
extern "C" {
// @symbol func_ov014_0211236c
void func_ov014_0211236c(daWanwan_c *c)
{
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
    z = c->mChainExtension;
    head.x = 0;
    head.y = 0;
    head.z = z;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;

    prevpos.x = c->mSpawnPosX;
    {
        int py = c->mSpawnPosY;
        prevpos.y = py;
        prevpos.z = c->mSpawnPosZ;
        prevpos.y = py + 0x1e000;
    }

    z = c->mChainExtension;
    head.x = 0;
    head.y = 0;
    head.z = z;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;

    cursor = (u32)&c->mLinkPos[6];
    i = 6;
    do {
        cur = (Vector3 *)cursor;
        target = (i == 6) ? &prevpos : cur + 1;
        Vec3_Sub(&diff, cur, target);
        tmp.x = diff.x;
        tmp.y = diff.y;
        tmp.z = diff.z;
        /* cstd::atan2 takes Fix12<int> by value. */
        angY = _ZN4cstd5atan2E5Fix12IiES1_(tmp.x, tmp.z);
        angX = (short)(-_ZN4cstd5atan2E5Fix12IiES1_(tmp.y, Vec3_HorzLen(&tmp)));
        Matrix4x3_FromRotationY(&data_020a0e68, angY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, angX);
        MulVec3Mat4x3(&head, &data_020a0e68, &rotated);
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
}

/* Blend the actor position into the chain. Integer (7-i)/7 is 1 only for i == 0. */
extern "C" {
// @symbol func_ov014_021122dc
void func_ov014_021122dc(daWanwan_c *c)
{
    Vector3 *p;
    int i;
    Vector3 diff;
    Vector3 scaled;

    Vec3_Sub(&diff, &c->mPosX, &c->mLinkPos[0]);
    p = &c->mLinkPos[0];
    i = 0;
    do {
        Vec3_MulScalar(&scaled, &diff, (7 - i) / 7);
        AddVec3(p, &scaled, p);
        i++;
        p++;
    } while (i < 7);
}
}

/* Hit response. Egg (actor 9, hit 0x2000) pops the scale into state 0.
 * Explosion (0x4000) and a Bob-omb (0xce) go to state 4. A mega hit from
 * the player (0xbf, hit 0x10) starts the bite (state 5) and hurts when facing. */
extern "C" {
// @symbol func_ov014_02112114
void func_ov014_02112114(daWanwan_c *c)
{
    dActor_c *other;
    unsigned id;
    int ang;

    if ((unsigned)(c->mState - 3) <= 1)
        return;
    id = c->mdCcAcPos_c.otherOwner;
    if (id == 0)
        return;
    other = dActor_c::FindWithID(id);
    if (other == 0)
        return;

    /* Each actor-ID test turns the comparison into 0/1 and tests that
       against 0 (moveq/movne/cmp). A plain `==` drops those three
       instructions, 12 bytes shorter per test. */
    if ((int)(other->actorID == kYoshiEggId) != 0) {
        if (c->mdCcAcPos_c.hitFlags & 0x2000) {
            c->mScaleX = 0x2000;
            c->mScaleY = c->mScaleX;
            c->mScaleZ = c->mScaleY;
            c->func_ov014_02111ebc(0);
            return;
        }
    }

    if (c->mdCcAcPos_c.hitFlags & 0x4000) {
        c->mScaleX = 0x2000;
        c->mScaleY = c->mScaleX;
        c->mScaleZ = c->mScaleY;
        c->func_ov014_02111ebc(4);
        return;
    }

    if ((int)(other->actorID == kPlayerActorId) != 0) {
        ang = Vec3_HorzAngle(&c->mPos, &other->mPos);
        if (c->mdCcAcPos_c.hitFlags & 0x10) {
            c->func_ov014_02111ebc(5);
            c->mPrevAngleY = other->mAngleY;
        }
        if (AngleDiff(ang, c->mAngleY) < 0x4000) {
            Vector3 v;
            v.x = c->mPosX;
            v.y = c->mPosY;
            v.z = c->mPosZ;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
                (Player *)other, &v, 3, 0xc000, 1, 0, 1);
        }
    }

    if (c->mState == 4)
        return;
    if ((int)(other->actorID == kBobOmbActorId) != 0) {
        func_ov102_0214ae1c(other);
        c->func_ov014_02111ebc(4);
    }
}
}

/* Pull the chomp back inside chainExtension * 7 + 200 of the spawn point.
 * A fully extended lunge (state 2, extension 0x64000) also kills both speeds. */
extern "C" {
// @symbol func_ov014_02111fe0
void func_ov014_02111fe0(daWanwan_c *c)
{
    Vector3 v;
    Vector3 out;
    int len;
    int lim;
    Vec3_Sub(&v, &c->mPos, (Vector3 *)&c->mSpawnPosX);
    len = LenVec3(&v);
    lim = c->mChainExtension * 7 + kRestLift;
    if (len <= lim)
        return;
    v.x = FX_MUL(v.x, cstd::fdiv(lim, len));
    v.y = FX_MUL(v.y, cstd::fdiv(lim, len));
    v.z = FX_MUL(v.z, cstd::fdiv(lim, len));
    Vec3_Add(&out, (Vector3 *)&c->mSpawnPosX, &v);
    c->mPosX = out.x;
    c->mPosY = out.y;
    c->mPosZ = out.z;
    if (c->mState != 2)
        return;
    if (c->mChainExtension == kChainMax) {
        c->mHorzSpeed = 0;
        c->mVertSpeed = 0;
    }
}
}

// @symbol func_ov014_02111fb8
/* Landing one-shot. Sound 0x39 at camera-space position, then the dust. */
extern "C" void func_ov014_02111fb8(daWanwan_c *c)
{
    func_0201267c(0x39, &c->mCamSpacePosX);
    c->HugeLandingDust(true);
}

// @symbol func_ov014_02111f54
/* Start the release cutscene when the stump's mStepsLeft is 0 and the player
 * accepts the no-control lock. */
extern "C" int func_ov014_02111f54(daWanwan_c *self)
{
    daObjPile_c *stump = (daObjPile_c *)dActor_c::FindWithID((unsigned)self->mStumpUniqueID);
    if (stump->mStepsLeft != 0)
        goto fail;
    if (self->ClosestPlayer()->SetNoControlState(4, -1, 0) == 0)
        goto fail;
    self->func_ov014_02111ebc(3);
    self->mChainBroken = 1;
    return 1;
fail:
    return 0;
}

/* CodeWarrior's pointer-to-member is 8 bytes for daWanwan_c, a complete
 * single-inheritance class here, so each row of the state table
 * data_ov014_0211476c is 20 bytes: enter PMF at +0, update PMF at +8, name
 * pointer at +0x10. The table is defined at the bottom of this file; the
 * initializer copies each row's enter/update pair by value from twelve 8-byte
 * PMF descriptors in .data at 021146a0..021146f8 (the name pointers are the
 * only static part). Measured from those descriptors and the name pointers:
 *   0  enter 02111e74  update 02111e14   scale back to 1.0     WAIT
 *   1  enter 02111dc4  update 02111ca8   idle, face the player  WALK
 *   2  enter 02111b70  update 02111af0   lunge                  ATTACK
 *   3  enter 02111a6c  update 021115ec   break chain and fence  ESCAPE_DEMO
 *   4  enter 021115c0  update 0211150c   knocked                BOMB_DAMAGE
 *   5  enter 021114d8  update 02111484   bite                   GIANT_DAMAGE
 * 02111ebc reads the PMF at +0 (enter). 02111f08 reads the PMF at +8 (update).
 */
typedef void (daWanwan_c::*WanwanPmf)();

/* The descriptor objects the initializer copies into the table. They stay
 * ROM-supplied data; only their addresses are referenced here. */
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

/* The state-name strings the table's name pointers reference. Also
 * ROM-supplied .data. */
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

// @symbol _ZN10daWanwan_c19func_ov014_02111f08Ev
void daWanwan_c::func_ov014_02111f08()
{
    (this->*data_ov014_0211476c[mState].update)();
}

// @symbol _ZN10daWanwan_c19func_ov014_02111ebcEi
void daWanwan_c::func_ov014_02111ebc(int i)
{
    mState = i;
    (this->*data_ov014_0211476c[mState].enter)();
}

// @symbol func_ov014_02111e74
/* State 0 enter: stop, hold for 0x78 frames, play the idle BCA. */
extern "C" void func_ov014_02111e74(daWanwan_c *c)
{
    c->mVertSpeed = 0;
    c->mHorzSpeed = 0;
    c->mActionTimer = 0x78;
    SetChompAnim(c, data_ov014_02114980);
}

// @symbol func_ov014_02111e14
/* State 0 update: scale back to 1.0, then idle. */
extern "C" void func_ov014_02111e14(daWanwan_c *c)
{
    c->mVertSpeed = 0;
    ApproachLinear(c->mScaleX, 0x1000, 0x500);
    c->mScaleZ = c->mScaleX;
    c->mScaleY = c->mScaleZ;
    if (DecIfAbove0_Short(&c->mActionTimer) != 0)
        return;
    c->func_ov014_02111ebc(1);
}

// @symbol func_ov014_02111dc4
/* State 1 enter: gravity back on, idle BCA, 0x78-frame pause. */
extern "C" void func_ov014_02111dc4(daWanwan_c *c)
{
    c->mVertSpeed = 0;
    c->mVertAccel = -0x2000;
    c->mActionTimer = 0x78;
    SetChompAnim(c, data_ov014_02114980);
}

// @symbol func_ov014_02111ca8
/* State 1 update. Tries the release, turns toward mTargetAngY, and on the
 * ground lunges (state 2) when the player is close and in front. */
extern "C" void func_ov014_02111ca8(daWanwan_c *c)
{
    if (func_ov014_02111f54(c))
        return;
    ApproachAngle(&c->mAngleX, 0, 4, 0x200, 0x80);
    DecIfAbove0_Short(&c->mActionTimer);
    ApproachLinear(c->mChainExtension, kChainSlack, 0x1000);
    ApproachLinear(c->mAngleY, c->mTargetAngY, 0x190);
    ApproachLinear(c->mHorzSpeed, 0, 0x400);
    if (c->mIsOnGround) {
        int d = c->DistToCPlayer();
        c->mTargetAngY = c->HorzAngleToCPlayer();
        c->mPrevAngleY = c->mAngleY;
        c->mHorzSpeed = 0xa000;
        c->mVertSpeed = 0x14000;
        c->unk_600 = 0;
        if (d < 0x500000 &&
            AngleDiff(c->mTargetAngY, c->mAngleY) < 0x800 &&
            c->mActionTimer == 0) {
            c->func_ov014_02111ebc(2);
        }
    }
    static_cast<dExtFrameCtrl_c &>(c->mModelAnim).Advance();
}

// @symbol func_ov014_02111b70
/* State 2 enter: sound 0x3a, lunge BCA, launch along the vertical angle to
 * the player (raised 0x50000). The third argument of the sound call is real
 * in this body; the two-argument definition ignores it. */
extern "C" void func_ov014_02111b70(daWanwan_c *c)
{
    int tgt[3];
    Player *player;
    int a;
    int i;
    int v;

    c->mVertAccel = 0;
    call3_267c::func_0201267c(0x3a, &c->mCamSpacePosX, 0);
    SetChompAnim(c, data_ov014_02114970);
    player = c->ClosestPlayer();

    /* ROM load order: y, z, x — then y+0x50000, store x, setup call, store y/z */
    {
        int y = player->mPosY;
        int z = player->mPosZ;
        int x = player->mPosX;
        tgt[0] = x;
        tgt[1] = y + 0x50000;
        tgt[2] = z;
    }

    a = (unsigned short)Vec3_VertAngle(&c->mPosX, tgt);
    i = (a >> 4) << 1;
    c->mVertSpeed = -FX_MUL(data_02082214[i], 0x8c000);
    v = c->mVertSpeed;
    if (v < 0x5000)
        v = 0x5000;
    else if (v > 0x2d000)
        v = 0x2d000;
    c->mVertSpeed = v;
    c->mHorzSpeed = FX_MUL(data_02082214[i + 1], 0x8c000);
    c->mActionTimer = 0x3c;
}

// @symbol func_ov014_02111af0
/* State 2 update: stretch the leash to kChainMax, then back to idle. */
extern "C" int func_ov014_02111af0(daWanwan_c *c)
{
    int r = func_ov014_02111f54(c);
    if (r)
        return r;
    if (Math_Function_0203b14c(&c->mChainExtension, kChainMax, 0x800, 0x10000, 0x800))
        goto adv;
    if (DecIfAbove0_Short(&c->mActionTimer))
        goto adv;
    c->func_ov014_02111ebc(1);
adv:
    static_cast<dExtFrameCtrl_c &>(c->mModelAnim).Advance();
}

// @symbol func_ov014_02111a6c
/* State 3 enter: idle BCA, snap back to the rest height, clear clip flags. */
extern "C" void func_ov014_02111a6c(daWanwan_c *c)
{
    SetChompAnim(c, data_ov014_02114980);
    c->mVertSpeed = 0;
    c->mHorzSpeed = 0;
    c->mVertAccel = -0x2000;
    c->mReleaseStep = 0;
    c->unk_600 = 0;
    c->mActionTimer = 0x3c;
    c->mFlags &= ~3;
    c->mPosY = c->mSpawnPosY + kRestLift;
}

/* State 3 update: the fence-break cutscene. mReleaseStep is the phase. */
static inline void incRelease(daWanwan_c *self)
{
    u8 *p = &self->mReleaseStep;
    *p = (u8)(*p + 1);
}

// @symbol func_ov014_021115ec
extern "C" void func_ov014_021115ec(daWanwan_c *self)
{
    /* local extern: no header this file includes declares it. Nonzero once
       the position has reached the target. */
    s32 Vec3_ApproachHorz(Vector3 *out, Vector3 *target, s32 maxStep);

    daObjWanwanShutter_c *fence;
    Vector3 partnerPos;
    s16 angleToPlayer;
    s16 angleToAnchor;
    dCamera_c *camera;

    Sound::PlaySecretSound(self, &self->mSecretSound);
    fence = (daObjWanwanShutter_c *)dActor_c::FindWithID((unsigned)self->mFenceUniqueID);
    {
        s32 *src = &fence->mPosX;
        s32 fifth = 0x80;
        partnerPos.x = src[0];
        short *ap = &self->mAngleX;
        partnerPos.y = src[1];
        s32 z = 0;
        partnerPos.z = src[2];
        ApproachAngle(ap, z, 4, 0x200, fifth);
    }
    camera = (dCamera_c *)data_0209f318;
    angleToPlayer = self->HorzAngleToCPlayer();
    angleToAnchor = Vec3_HorzAngle(&self->mPos, (Vector3 *)&self->mSpawnPosX);
    /* Steps 0-6 make the chomp the camera's mFocusActor. */
    switch (self->mReleaseStep) {
    case 0:
        camera->mFocusActor = self;
        if (ApproachLinear(self->mAngleY, angleToPlayer, 0x320) != 0
            && DecIfAbove0_Short(&self->mActionTimer) == 0)
            incRelease(self);
        self->mPrevAngleY = self->mAngleY;
        break;
    case 1: case 2: case 3: case 4:
        camera->mFocusActor = self;
        ApproachLinear(self->mHorzSpeed, 0, 0x400);
        if (self->mIsOnGround != 0) {
            self->mPrevAngleY = (s16)(angleToAnchor + 0x2000);
            self->mAngleY = self->mPrevAngleY;
            self->mVertSpeed = 0x32000;
            self->mHorzSpeed = 0x1e000;
            incRelease(self);
        }
        break;
    case 5:
        camera->mFocusActor = self;
        ApproachLinear(self->mHorzSpeed, 0, 0x400);
        if (self->mIsOnGround != 0) {
            self->mPrevAngleY = angleToAnchor;
            self->mAngleY = self->mPrevAngleY;
            self->mVertSpeed = 0x32000;
            self->mHorzSpeed = 0x1e000;
            incRelease(self);
            if (fence->mDisabled != 0)
                self->mReleaseStep = 7;
        }
        break;
    case 6: {
        camera->mFocusActor = self;
        ApproachLinear(self->mReleaseSpeed, 0x1000, 0x400);
        if (self->mIsOnGround != 0) {
            SetChompAnim(self, data_ov014_02114970);
            self->mAngleY = Vec3_HorzAngle(&self->mPos, &partnerPos);
            self->mPrevAngleY = self->mAngleY;
            self->mVertSpeed = 0x14000;
            self->mReleaseSpeed = kChainMax;
        }
        {
            Vector3 target;
            s32 x = partnerPos.x;
            target.x = x;
            s32 z = partnerPos.z;
            target.z = z;
            s32 y = partnerPos.y;
            target.y = y;
            {
                s16 *tbl = data_02082214;
                u16 ang = fence->mAngleY;
                s32 s = tbl[(ang >> 4) << 1];
                s32 add = FX_MUL(s, 0x96000);
                target.x = x + add;
            }
            {
                s16 *tbl = data_02082214;
                u16 ang = fence->mAngleY;
                s32 s = tbl[(((ang >> 4) << 1) + 1)];
                s32 add = FX_MUL(s, 0x96000);
                target.z = z + add;
            }
            if (Vec3_ApproachHorz(&self->mPos, &target, self->mReleaseSpeed) != 0) {
                incRelease(self);
                self->mPrevAngleY = (s16)(Vec3_HorzAngle(&self->mPos, &partnerPos) + 0x8000);
                self->mHorzSpeed = 0x28000;
                self->mVertSpeed = 0xa000;
                func_ov014_02112ea8(fence);
            }
        }
        break;
    }
    case 7:
        if (self->mIsOnGround != 0) {
            SetChompAnim(self, data_ov014_02114980);
            self->mPrevAngleY = Vec3_HorzAngle(&self->mPos, &partnerPos);
            self->mActionTimer = 0x3c;
            self->mAngleY = self->mPrevAngleY;
            if (fence->mDisabled != 0) {
                self->mHorzSpeed = 0x28000;
                self->mVertSpeed = 0x5a000;
            } else {
                self->mHorzSpeed = 0x1e000;
                self->mVertSpeed = kChainSlack;
            }
            camera->SetFlag_3();
            incRelease(self);
        }
        break;
    case 8:
        if (DecIfAbove0_Short(&self->mActionTimer) == 0) {
            camera->mFlags &= ~8u;
            if (self->ClosestPlayer()->Unk_020ca150(4) != 0) {
                incRelease(self);
                self->mActionTimer = 0x3c;
            }
        } else {
            camera->SetLookAt(self->mPos);
        }
        break;
    case 9:
        if (DecIfAbove0_Short(&self->mActionTimer) == 0)
            self->MarkForDestruction();
        break;
    }
    static_cast<dExtFrameCtrl_c &>(self->mModelAnim).Advance();
}

// @symbol func_ov014_021115c0
/* State 4 enter: sound 0x3a and a hard upward speed. */
extern "C" void func_ov014_021115c0(daWanwan_c *c)
{
    func_0201267c(0x3a, &c->mCamSpacePosX);
    c->mVertSpeed = 0x12c000;
    c->mVertAccel = 0;
}

// @symbol func_ov014_0211150c
/* State 4 update. The early exits fall out of the bottom: the ROM leaves r0
 * alone on those paths, and a valueless return is not available to the host. */
extern "C" int func_ov014_0211150c(daWanwan_c *c)
{
    ApproachLinear(c->mScaleX, 0x1000, 0x500);
    c->mScaleZ = c->mScaleX;
    c->mScaleY = c->mScaleZ;
    ApproachAngle(&c->mAngleX, -0x4000, 4, 0x1000, 0x400);
    if (Math_Function_0203b14c(&c->mChainExtension, kChainMax, 0x800, 0x10000, 0x800) == 0) {
        if (DecIfAbove0_Short(&c->mActionTimer) == 0)
            c->func_ov014_02111ebc(1);
    }
}

// @symbol func_ov014_021114d8
/* State 5 enter: face mPrevAngleY and launch. */
extern "C" void func_ov014_021114d8(daWanwan_c *c)
{
    c->mAngleY = c->mPrevAngleY;
    c->mActionTimer = 0x3c;
    c->mHorzSpeed = kChainMax;
    c->mVertSpeed = kRestLift;
    c->mVertAccel = -0xa000;
}

// @symbol func_ov014_02111484
/* State 5 update: back to idle when the timer ends. On the ground the
 * vertical speed copies the horizontal one, which then brakes. */
extern "C" void func_ov014_02111484(daWanwan_c *c)
{
    if (DecIfAbove0_Short(&c->mActionTimer) == 0)
        c->func_ov014_02111ebc(1);
    if (c->mIsOnGround != 0)
        c->mVertSpeed = c->mHorzSpeed;
    ApproachLinear(c->mHorzSpeed, 0, 0x2000);
}

// @symbol _ZN10daWanwan_cD0Ev
// @symbol _ZN10daWanwan_cD1Ev
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
    { data_ov014_021146e0, data_ov014_021146b8, data_ov014_02114690 },
    { data_ov014_021146f0, data_ov014_021146c0, data_ov014_02114688 },
    { data_ov014_021146a8, data_ov014_021146a0, data_ov014_02114698 },
    { data_ov014_021146f8, data_ov014_021146b0, data_ov014_0211470c },
    { data_ov014_021146d8, data_ov014_021146c8, data_ov014_02114724 },
    { data_ov014_021146e8, data_ov014_021146d0, data_ov014_02114730 },
};
