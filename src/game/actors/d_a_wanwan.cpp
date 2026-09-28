//cpp
/**
 * daWanwan_c -- Bob-omb Battlefield Chain Chomp. Seven links, the stump it
 * spawns (STUMP 0x1b, daObjPile_c) and the fence it finds
 * (CHAIN_CHOMP_FENCE 0x29, daObjWanwanShutter_c).
 *
 * common.h stays first. Matrix4x3 has two 0x30-byte spellings, and the
 * shadow placement copies twelve words of whichever one is already in scope.
 *
 * g_profile_WANWAN is not defined here. The func_ov014_* bodies stay free
 * functions: the sinit installs them as 8-byte PMFs in data_ov014_0211476c,
 * enter at +0 and update at +8, and this file does not own that table.
 */

#include "common.h"
#include "daWanwan_c.h"
#include "daObjPile_c.h"
#include "daObjWanwanShutter_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Camera.h"
#include "Sound.h"
#include "dCc_c.h"
#include "Animation.h"

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

/* Two words, from this TU's LoadFile / SetFile / SetAnim uses. SharedFilePtr
 * has no fields; +4 is the BMD or BCA the load just filled in.
 *   02114968  body BMD   sinit 0x9c02, Model::LoadFile, mModelAnim
 *   02114978  link BMD   sinit 0x9c01, Model::LoadFile, mLinkModels
 *   02114980  idle BCA   sinit 0x9c04, Animation::LoadFile
 *   02114970  lunge BCA  sinit 0x9c03, Animation::LoadFile
 */
struct Ov014Loaded {
    u32 id;
    void *file;
};
#define ov014_loaded(handle) (((Ov014Loaded *)&(handle))->file)

extern "C" {
extern SharedFilePtr data_ov014_02114968;
extern SharedFilePtr data_ov014_02114978;
extern SharedFilePtr data_ov014_02114980;
extern SharedFilePtr data_ov014_02114970;
extern int data_ov014_02114700[]; /* collision offset (0, -200, 0) */
extern Matrix4x3 data_020a0e68;
extern const Matrix4x3 IDENTITY_MATRIX4X3;
extern s16 data_02082214[];
extern void *data_0209f318;

extern int func_ov014_02111fb8(char *c);
void func_ov014_02112114(void *c);
void func_ov014_02111fe0(char *c);
void func_ov014_0211250c(char *c);
void func_ov014_0211236c(char *c);
void func_ov014_021122dc(char *c);
void func_ov014_02112788(char *c);
void func_ov014_02112ea8(void *fence);
void func_ov102_0214ae1c(void *bomb);

/* Scalar stand-ins. The header methods take Fix12<int> by value. */
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *pos, int radius, int height,
    unsigned flags, unsigned vuln);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *self, ShadowModel *shadow, Matrix4x3 *mtx, int radius, int depth,
    unsigned opacity);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, void *bca, int flags, int speed, unsigned start);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    Player *player, const Vector3 *pos, unsigned kind, int power, unsigned a,
    unsigned b, unsigned c);
short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
void _ZN6Camera9SetFlag_3Ev(Camera *cam);

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

extern "C" {

dEnemyBase_c *_ZN12dEnemyBase_cC2Ev(dEnemyBase_c *object);
dCcAcPos_c *_ZN10dCcAcPos_cC1Ev(dCcAcPos_c *object);
ModelAnim *_ZN9ModelAnimC1Ev(ModelAnim *object);
ShadowModel *_ZN11ShadowModelC1Ev(ShadowModel *object);
void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
    void (*ctor)(void *), void (*dtor)(void *));
extern int _ZTV10daWanwan_c[];
extern Model *_ZN5ModelD1Ev(Model *object);
extern Model *_ZN5ModelC1Ev(Model *object);
extern ShadowModel *_ZN11ShadowModelD1Ev(ShadowModel *object);
extern Vector3 *_ZN7Vector3D1Ev(Vector3 *object);
extern void func_0203d384(void);
}

namespace cstd {
int fdiv(int a, int b);
}

namespace call3_267c {
extern "C" int func_0201267c(int id, void *pos, int unused);
}

/* ModelAnim::SetAnim(BCA_File *, int, Fix12<int>, u32) measured +8 bytes:
   02111e74 0x48->0x50, 02111dc4 0x50->0x58, 02111a6c 0x84->0x8c,
   02111b70 0x138->0x140. The scalar extern stays. */
#define SetChompAnim(self, bca) \
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj( \
        &(self)->mModelAnim, ov014_loaded(bca), 0, 0x1000, 0)

// @symbol daWanwan_c_classInit
/* Hand-rolled. The two Vector3[7] arrays are constructed by func_0203d384,
 * not by Vector3's implicit default. */
/* return new daWanwan_c() measured 0xf0->0xa0, and the vec_ctor slot
   relocates ShadowModelD1 where the ROM still has 0x020733a8. func_0203d384
   stays the Vector3[7] constructor. */
extern "C" daWanwan_c *daWanwan_c_classInit()
{
    daWanwan_c *c = (daWanwan_c *)fBase_c::operator new(0x620);
    if (c) {
        _ZN12dEnemyBase_cC2Ev(c);
        *(int **)c = &_ZTV10daWanwan_c[2];
        _ZN10dCcAcPos_cC1Ev(&c->mdCcAcPos_c);
        _ZN9ModelAnimC1Ev(&c->mModelAnim);
        _ZN11ShadowModelC1Ev(&c->mShadowModel);
        __cxa_vec_ctor(c->mLinkModels, 7, 0x50,
            (void (*)(void *))_ZN5ModelC1Ev, (void (*)(void *))_ZN5ModelD1Ev);
        __cxa_vec_ctor(c->mLinkShadows, 7, 0x28,
            (void (*)(void *))_ZN11ShadowModelC1Ev,
            (void (*)(void *))_ZN11ShadowModelD1Ev);
        __cxa_vec_ctor(c->mLinkPos, 7, 0xc,
            (void (*)(void *))func_0203d384, (void (*)(void *))_ZN7Vector3D1Ev);
        __cxa_vec_ctor(c->mLinkDelta, 7, 0xc,
            (void (*)(void *))func_0203d384, (void (*)(void *))_ZN7Vector3D1Ev);
    }
    return c;
}

// @symbol _ZN10daWanwan_c13InitResourcesEv
int daWanwan_c::InitResources()
{
    void *f = Model::LoadFile(data_ov014_02114968);
    mModelAnim.SetFile((BMD_File *)f, 1, 1);
    Model::LoadFile(data_ov014_02114978);
    Animation::LoadFile(data_ov014_02114980);
    Animation::LoadFile(data_ov014_02114970);

    {
        int i = 0;
        unsigned char *p = (unsigned char *)mLinkModels;
        do {
            ((Model *)p)->SetFile(
                (BMD_File *)ov014_loaded(data_ov014_02114978), 1, 1);
            i = i + 1;
            p = p + 0x50;
        } while (i < 7);
    }

    mShadowModel.InitCylinder();
    {
        int si = 0;
        unsigned char *sp = (unsigned char *)mLinkShadows;
        do {
            ((ShadowModel *)sp)->InitCylinder();
            si = si + 1;
            sp = sp + 0x28;
        } while (si < 7);
    }

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;

    {
        int v[3];
        v[0] = data_ov014_02114700[0];
        v[1] = data_ov014_02114700[1];
        v[2] = data_ov014_02114700[2];
        /* dCcAcPos_c::Init(Fix12<int>, Fix12<int>) measured 0x208->0x218. */
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            &mdCcAcPos_c, this, (Vector3 *)v, 0x96000, 0x12c000, 0x200004, 0x26ff0);
    }

    func_ov014_02111ebc(1);

    {
        /* dst stays the actor. The stores are [dst, #0x524] -- mLinkPos -- and
           a pointer that already points at the array is two instructions longer. */
        int cnt = 0;
        unsigned char *dst = (unsigned char *)this;
        do {
            *(int *)(dst + 0x524) = mPosX;
            cnt = cnt + 1;
            *(int *)(dst + 0x528) = mPosY;
            *(int *)(dst + 0x52c) = mPosZ;
            dst = dst + 0xc;
        } while (cnt < 7);
    }

    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;

    dActor_c *spawned = dActor_c::Spawn(
        kStumpActorId, 0x11, *(Vector3 *)&mPosX, 0, mAreaId, -1);
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
                func_ov014_02111fb8((char *)this);
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
        func_ov014_02111fe0((char *)this);
    func_ov014_0211250c((char *)this);
    if (mChainBroken == 0) {
        func_ov014_0211236c((char *)this);
        func_ov014_021122dc((char *)this);
    }
    func_ov014_02112788((char *)this);
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
    mModelAnim.Render((const Vector3 *)&mScaleX);

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
void func_ov014_02112788(char *c)
{
    /* Head matrix is mModelAnim.mat4x3 (actor+0x16c); translation is m[9..11]
       at +0x190. Link models sit at +0x1dc, their translation at o+0x21c while
       o walks from the actor by 0x50. e+0x524 is mLinkPos. A named base for
       those stores changes the register the loop keeps live. */
    Matrix4x3 tmp;
    int i;
    int t;
    char *m;
    char *e;
    char *o;
    char *sm;
    Matrix4x3_FromRotationXYZExt(c + 0x16c, *(s16 *)(c + 0x8c), *(s16 *)(c + 0x8e), *(s16 *)(c + 0x90));
    *(int *)(c + 0x190) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x194) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x198) = *(int *)(c + 0x64) >> 3;
    t = *(int *)(c + 0x60) - *(int *)(c + 0x5f0);
    if (t <= 0x1000)
        t = 0x1000;
    /* DropShadowRadHeight(Fix12<int>, Fix12<int>) measured 0x1c4->0x1e4 for both calls. */
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        (dActor_c *)c, (ShadowModel *)(c + 0x1b4), (Matrix4x3 *)(c + 0x16c),
        0x15e000 - (int)(((long long)t * 0x180 + 0x800) >> 12),
        t + 0x28000,
        0xf);
    tmp = IDENTITY_MATRIX4X3;
    sm = c + 0x40c;
    i = 0;
    m = c + 0x1dc;
    e = c;
    o = c;
    for (; i < 7; i++) {
        *(Matrix4x3 *)(m + 0x1c) = tmp;
        *(int *)(o + 0x21c) = *(int *)(e + 0x524) >> 3;
        *(int *)(o + 0x220) = *(int *)(e + 0x528) >> 3;
        *(int *)(o + 0x224) = *(int *)(e + 0x52c) >> 3;
        t = *(int *)(e + 0x528) - *(int *)(c + 0x5f0);
        if (t <= 0x1000)
            t = 0x1000;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            (dActor_c *)c, (ShadowModel *)sm, (Matrix4x3 *)(m + 0x1c),
            0x78000 - (int)(((long long)t * 0x180 + 0x800) >> 12),
            t + 0x28000,
            0xf);
        m += 0x50;
        e += 0xc;
        o += 0x50;
        sm += 0x28;
    }
}
}

/* Forward chain: head at the actor, then links 1..6, carrying mLinkDelta. */
extern "C" {
// @symbol func_ov014_0211250c
typedef struct { int x, y, z; } WanwanVec;

void func_ov014_0211250c(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    int yoff;
    WanwanVec dvec;
    WanwanVec head;
    WanwanVec rotated;
    WanwanVec saved;
    WanwanVec tmp;
    WanwanVec sum;
    WanwanVec diff;
    WanwanVec mul;
    int scale;
    WanwanVec *seg;
    WanwanVec *dst;
    int *bound;
    int state;
    short angY;
    short angX;
    WanwanVec *prev;
    int i;

    head.x = 0;
    head.y = 0;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;
    head.z = -kRestLift;

    /* setup order is load-bearing: seg before state so add/cmp schedule matches */
    seg = (WanwanVec *)&c->mLinkPos[1];
    dst = (WanwanVec *)&c->mLinkDelta[1];
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
    Vec3_Add(&tmp, (WanwanVec *)&c->mPosX, &rotated);
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
            prev = (WanwanVec *)((char *)seg - 0xc);

        dvec.x = (seg->x - prev->x) + dst->x;
        dvec.z = (seg->z - prev->z) + dst->z;
        {
            int t = (seg->y + dst->y) + yoff;
            if (t <= *bound)
                t = *bound;
            dvec.y = t - prev->y;
        }

        /* cstd::atan2(Fix12<int>, Fix12<int>) measured 0x27c->0x2a4 on this function. */
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

        seg = (WanwanVec *)((char *)seg + 0xc);
        dst = (WanwanVec *)((char *)dst + 0xc);
    }
}
}

/* Backward chain, from the stump end (link 6) toward the head. */
extern "C" {
// @symbol func_ov014_0211236c
void func_ov014_0211236c(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    WanwanVec tmp;
    WanwanVec head;
    WanwanVec rotated;
    WanwanVec sum;
    WanwanVec prevpos;
    WanwanVec diff;
    WanwanVec delta;
    WanwanVec scaled;
    WanwanVec mul;
    short angY;
    WanwanVec *target;
    WanwanVec *cur;
    short angX;
    int i;
    int z;

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

    cur = (WanwanVec *)&c->mLinkPos[6];
    i = 6;
    do {
        target = (i == 6) ? &prevpos : (WanwanVec *)((char *)cur + 0xc);
        Vec3_Sub(&diff, cur, target);
        tmp.x = diff.x;
        tmp.y = diff.y;
        tmp.z = diff.z;
        /* cstd::atan2(Fix12<int>, Fix12<int>) measured 0x1a0->0x1c4 on this function. */
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
        i = i - 1;
        cur = (WanwanVec *)((char *)cur - 0xc);
    } while (i >= 0);
}
}

/* Blend the actor position into the chain. Integer (7-i)/7 is 1 only for i == 0. */
extern "C" {
// @symbol func_ov014_021122dc
void func_ov014_021122dc(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    WanwanVec *p;
    int i;
    int diff[3];
    int scaled[3];

    Vec3_Sub(diff, &c->mPosX, &c->mLinkPos[0].x);
    p = (WanwanVec *)&c->mLinkPos[0];
    i = 0;
    do {
        Vec3_MulScalar(scaled, diff, (7 - i) / 7);
        AddVec3(p, scaled, p);
        i++;
        p = (WanwanVec *)((char *)p + 0xc);
    } while (i < 7);
}
}

/* Hit response. Egg (actor 9, hit 0x2000) pops the scale into state 0.
 * Explosion (0x4000) and a Bob-omb (0xce) go to state 4. A mega hit from
 * the player (0xbf, hit 0x10) starts the bite (state 5) and hurts when facing. */
extern "C" {
// @symbol func_ov014_02112114
void func_ov014_02112114(void *cc)
{
    daWanwan_c *c = (daWanwan_c *)cc;
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
        ang = Vec3_HorzAngle((const Vector3 *)&c->mPosX, (const Vector3 *)&other->mPosX);
        if (c->mdCcAcPos_c.hitFlags & 0x10) {
            c->func_ov014_02111ebc(5);
            c->mPrevAngleY = other->mAngleY;
        }
        if (AngleDiff(ang, c->mAngleY) < 0x4000) {
            int v[3];
            v[0] = c->mPosX;
            v[1] = c->mPosY;
            v[2] = c->mPosZ;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
                (Player *)other, (Vector3 *)v, 3, 0xc000, 1, 0, 1);
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
void func_ov014_02111fe0(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    Vector3 v;
    Vector3 out;
    int len;
    int lim;
    Vec3_Sub(&v, (Vector3 *)&c->mPosX, (Vector3 *)&c->mSpawnPosX);
    len = LenVec3(&v);
    lim = c->mChainExtension * 7 + kRestLift;
    if (len <= lim)
        return;
    v.x = (int)(((s64)v.x * cstd::fdiv(lim, len) + 0x800) >> 12);
    v.y = (int)(((s64)v.y * cstd::fdiv(lim, len) + 0x800) >> 12);
    v.z = (int)(((s64)v.z * cstd::fdiv(lim, len) + 0x800) >> 12);
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
extern "C" int func_ov014_02111fb8(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    func_0201267c(0x39, &c->mCamSpacePosX);
    c->HugeLandingDust(1);
}

// @symbol func_ov014_02111f54
/* Start the release cutscene when the stump's mState is 0 and the player
 * accepts the no-control lock. */
extern "C" int func_ov014_02111f54(void *raw)
{
    daWanwan_c *self = (daWanwan_c *)raw;
    daObjPile_c *stump = (daObjPile_c *)dActor_c::FindWithID((unsigned)self->mStumpUniqueID);
    if (stump->mState != 0)
        goto fail;
    if (self->ClosestPlayer()->SetNoControlState(4, -1, 0) == 0)
        goto fail;
    self->func_ov014_02111ebc(3);
    self->mChainBroken = 1;
    return 1;
fail:
    return 0;
}

/* CodeWarrior's pointer-to-member is 8 bytes for a complete single-inheritance
 * class. MSVC picks 16 bytes if the class is still incomplete here, and then
 * `20 - 8 - sizeof(PMF)` does not fit. The class is complete above, so both
 * compilers agree. mwccarm's object does not change either way.
 *
 * sinit copies six pairs into data_ov014_0211476c. Each pair is an 8-byte PMF
 * (function, adjustment 0) measured from the rodata at 021146a0:
 *   0  enter 02111e74  update 02111e14   scale back to 1.0
 *   1  enter 02111dc4  update 02111ca8   idle, face the player
 *   2  enter 02111b70  update 02111af0   lunge
 *   3  enter 02111a6c  update 021115ec   break the chain and the fence
 *   4  enter 021115c0  update 0211150c   knocked
 *   5  enter 021114d8  update 02111484   bite
 * 02111ebc reads the PMF at +0 (enter). 02111f08 reads the PMF at +8 (update).
 */
struct WanwanState {
    char pad[0x610];
    int mState;
};
typedef void (WanwanState::*WanwanPmf)();
struct Entry {
    char pad[8];
    WanwanPmf pmf;
    char tail[20 - 8 - sizeof(WanwanPmf)];
};
extern Entry data_ov014_0211476c[];

// @symbol _ZN10daWanwan_c19func_ov014_02111f08Ev
void daWanwan_c::func_ov014_02111f08()
{
    WanwanState *c = (WanwanState *)this;
    int j = c->mState;
    (c->*data_ov014_0211476c[j].pmf)();
}

namespace ent0 {
struct Entry {
    void (WanwanState::*pmf)();
    char rest[12];
};
extern "C" Entry data_ov014_0211476c[];
}

// @symbol _ZN10daWanwan_c19func_ov014_02111ebcEi
void daWanwan_c::func_ov014_02111ebc(int i)
{
    WanwanState *c = (WanwanState *)this;
    c->mState = i;
    int j = c->mState;
    (c->*ent0::data_ov014_0211476c[j].pmf)();
}

// @symbol func_ov014_02111e74
/* State 0 enter: stop, hold for 0x78 frames, play the idle BCA. */
extern "C" void func_ov014_02111e74(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    c->mVertSpeed = 0;
    c->mHorzSpeed = 0;
    c->mActionTimer = 0x78;
    SetChompAnim(c, data_ov014_02114980);
}

// @symbol func_ov014_02111e14
/* State 0 update: scale back to 1.0, then idle. */
extern "C" void func_ov014_02111e14(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
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
extern "C" void func_ov014_02111dc4(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    c->mVertSpeed = 0;
    c->mVertAccel = -0x2000;
    c->mActionTimer = 0x78;
    SetChompAnim(c, data_ov014_02114980);
}

// @symbol func_ov014_02111ca8
/* State 1 update. Tries the release, turns toward mTargetAngY, and on the
 * ground lunges (state 2) when the player is close and in front. */
extern "C" void func_ov014_02111ca8(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
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
    static_cast<Animation &>(c->mModelAnim).Advance();
}

// @symbol func_ov014_02111b70
/* State 2 enter: sound 0x3a, lunge BCA, launch along the vertical angle to
 * the player (raised 0x50000). The third argument of the sound call is real
 * in this body; the two-argument definition ignores it. */
extern "C" void func_ov014_02111b70(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
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
    c->mVertSpeed = -((int)(((((long long)data_02082214[i]) * 0x8c000) + 0x800) >> 12));
    v = c->mVertSpeed;
    if (v < 0x5000)
        v = 0x5000;
    else if (v > 0x2d000)
        v = 0x2d000;
    c->mVertSpeed = v;
    c->mHorzSpeed = (int)(((((long long)data_02082214[i + 1]) * 0x8c000) + 0x800) >> 12);
    c->mActionTimer = 0x3c;
}

// @symbol func_ov014_02111af0
/* State 2 update: stretch the leash to kChainMax, then back to idle. */
extern "C" int func_ov014_02111af0(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    int r = func_ov014_02111f54(c);
    if (r)
        return r;
    if (Math_Function_0203b14c(&c->mChainExtension, kChainMax, 0x800, 0x10000, 0x800))
        goto adv;
    if (DecIfAbove0_Short(&c->mActionTimer))
        goto adv;
    c->func_ov014_02111ebc(1);
adv:
    static_cast<Animation &>(c->mModelAnim).Advance();
}

// @symbol func_ov014_02111a6c
/* State 3 enter: idle BCA, snap back to the rest height, clear clip flags. */
extern "C" void func_ov014_02111a6c(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
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
extern "C" void func_ov014_021115ec(u8 *raw)
{
    /* This body's parameter widths stay here. The header's void
     * Vec3_ApproachHorz does not describe these calls. */
    int ApproachAngle(void *angles, s32 a, s32 b, s32 c, s32 d);
    s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
    u16 DecIfAbove0_Short(void *p);
    s32 _Z14ApproachLinearRiii(void *dst, s32 target, s32 step);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self_, void *bca, s32 a, s32 fix, unsigned b);
    s32 Vec3_ApproachHorz(Vector3 *out, Vector3 *target, s32 maxStep);
    void func_ov014_02112ea8(void *actor);
    void _ZN6Camera9SetFlag_3Ev(void *cam);
    extern s16 data_02082214[];
    extern void *data_0209f318;

    daWanwan_c *self = (daWanwan_c *)raw;
    daObjWanwanShutter_c *fence;
    Vector3 partnerPos;
    s16 angleToPlayer;
    s16 angleToAnchor;
    Camera *camera;

    Sound::PlaySecretSound(self, &self->mSecretSound);
    fence = (daObjWanwanShutter_c *)dActor_c::FindWithID((unsigned)self->mFenceUniqueID);
    {
        s32 *src = &fence->mPosX;
        s32 fifth = 0x80;
        partnerPos.x = src[0];
        void *ap = &self->mAngleX;
        partnerPos.y = src[1];
        s32 z = 0;
        partnerPos.z = src[2];
        ApproachAngle(ap, z, 4, 0x200, fifth);
    }
    camera = (Camera *)data_0209f318;
    angleToPlayer = self->HorzAngleToCPlayer();
    angleToAnchor = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->mSpawnPosX);
    switch (self->mReleaseStep) {
    case 0:
        *(daWanwan_c **)((char *)camera + 0x118) = self;
        if (ApproachLinear(self->mAngleY, angleToPlayer, 0x320) != 0
            && DecIfAbove0_Short(&self->mActionTimer) == 0)
            incRelease(self);
        self->mPrevAngleY = self->mAngleY;
        break;
    case 1: case 2: case 3: case 4:
        *(daWanwan_c **)((char *)camera + 0x118) = self;
        _Z14ApproachLinearRiii(&self->mHorzSpeed, 0, 0x400);
        if (self->mIsOnGround != 0) {
            self->mPrevAngleY = (s16)(angleToAnchor + 0x2000);
            self->mAngleY = self->mPrevAngleY;
            self->mVertSpeed = 0x32000;
            self->mHorzSpeed = 0x1e000;
            incRelease(self);
        }
        break;
    case 5:
        *(daWanwan_c **)((char *)camera + 0x118) = self;
        _Z14ApproachLinearRiii(&self->mHorzSpeed, 0, 0x400);
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
        *(daWanwan_c **)((char *)camera + 0x118) = self;
        _Z14ApproachLinearRiii(&self->mReleaseSpeed, 0x1000, 0x400);
        if (self->mIsOnGround != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                &self->mModelAnim, ov014_loaded(data_ov014_02114970), 0, 0x1000, 0);
            self->mAngleY = Vec3_HorzAngle((Vector3 *)&self->mPosX, &partnerPos);
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
                s32 add = (s32)(((((long long)s) * 0x96000) + 0x800) >> 12);
                target.x = x + add;
            }
            {
                s16 *tbl = data_02082214;
                u16 ang = fence->mAngleY;
                s32 s = tbl[(((ang >> 4) << 1) + 1)];
                s32 add = (s32)(((((long long)s) * 0x96000) + 0x800) >> 12);
                target.z = z + add;
            }
            if (Vec3_ApproachHorz((Vector3 *)&self->mPosX, &target, self->mReleaseSpeed) != 0) {
                incRelease(self);
                self->mPrevAngleY = (s16)(Vec3_HorzAngle((Vector3 *)&self->mPosX, &partnerPos) + 0x8000);
                self->mHorzSpeed = 0x28000;
                self->mVertSpeed = 0xa000;
                func_ov014_02112ea8(fence);
            }
        }
        break;
    }
    case 7:
        if (self->mIsOnGround != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                &self->mModelAnim, ov014_loaded(data_ov014_02114980), 0, 0x1000, 0);
            self->mPrevAngleY = Vec3_HorzAngle((Vector3 *)&self->mPosX, &partnerPos);
            self->mActionTimer = 0x3c;
            self->mAngleY = self->mPrevAngleY;
            if (fence->mDisabled != 0) {
                self->mHorzSpeed = 0x28000;
                self->mVertSpeed = 0x5a000;
            } else {
                self->mHorzSpeed = 0x1e000;
                self->mVertSpeed = kChainSlack;
            }
            _ZN6Camera9SetFlag_3Ev(camera);
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
            camera->SetLookAt(*(Vector3 *)&self->mPosX);
        }
        break;
    case 9:
        if (DecIfAbove0_Short(&self->mActionTimer) == 0)
            self->MarkForDestruction();
        break;
    }
    static_cast<Animation &>(self->mModelAnim).Advance();
}

// @symbol func_ov014_021115c0
/* State 4 enter: sound 0x3a and a hard upward speed. */
extern "C" void func_ov014_021115c0(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    func_0201267c(0x3a, &c->mCamSpacePosX);
    c->mVertSpeed = 0x12c000;
    c->mVertAccel = 0;
}

// @symbol func_ov014_0211150c
/* State 4 update. The early exits fall out of the bottom: the ROM leaves r0
 * alone on those paths, and a valueless return is not available to the host. */
extern "C" int func_ov014_0211150c(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
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
extern "C" void func_ov014_021114d8(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    c->mAngleY = c->mPrevAngleY;
    c->mActionTimer = 0x3c;
    c->mHorzSpeed = kChainMax;
    c->mVertSpeed = kRestLift;
    c->mVertAccel = -0xa000;
}

// @symbol func_ov014_02111484
/* State 5 update: back to idle when the timer ends. On the ground the
 * vertical speed copies the horizontal one, which then brakes. */
extern "C" void func_ov014_02111484(char *raw)
{
    daWanwan_c *c = (daWanwan_c *)raw;
    if (DecIfAbove0_Short(&c->mActionTimer) == 0)
        c->func_ov014_02111ebc(1);
    if (c->mIsOnGround != 0)
        c->mVertSpeed = c->mHorzSpeed;
    ApproachLinear(c->mHorzSpeed, 0, 0x2000);
}

// @symbol _ZN10daWanwan_cD0Ev
// @symbol _ZN10daWanwan_cD1Ev
/* Both variants come from the inline destructor in daWanwan_c.h. Inline,
 * mwccarm emits D1 then D0 and no D2, which is the ROM order. */
