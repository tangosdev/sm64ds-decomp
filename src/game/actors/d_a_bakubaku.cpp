//cpp
/**
 * daBakubaku_c -- Bubba, the fish that patrols Jolly Roger Bay.
 *
 * mState points at one of five { enter, main } State records in this
 * overlay's .bss, filled by __sinit_d_a_bakubaku.cpp from the PMF
 * constants at 0x0211377c. Behavior runs state->main; transitions go
 * through func_ov032_02111ff4, which stores the record and runs enter.
 *
 *   02113a8c wander   enter 02111f9c  main 02111e24
 *   02113a9c pause    enter 02111dd8  main 02111d7c
 *   02113aac chase    enter 02111d58  main 02111b9c
 *   02113a7c surface  enter 02111814  main 02111620   (vert angle > 0)
 *   02113abc dive     enter 02111b50  main 02111830   (vert angle <= 0)
 *
 * The same sinit constructs the resource handles: 02113a40 model 0x295,
 * 02113a48 bite anim 0x296, 02113a50 swim anim 0x297 (the word at +4
 * is the loaded BCA/BMD). data_0209f32c is the stage water height
 * (daObjC0Water_c publishes it); data_ov032_021137cc / 021137d8 are
 * the body and head cylinder offsets.
 */
/* leftovers: common.h precedes daBakubaku_c.h to retain the current flat
 * Matrix4x3 copies. Actor, spawn and target positions remain separate scalar
 * fields; the existing vector pointer views at those helper boundaries are
 * still partial reconstruction. The state timer uses unsigned halfword reads,
 * and the state installer retains its StateFn pointer read-back.
 * Native Fix12<int> calls for collision setup, animation, shadow and Hurt
 * remain pending; the local declarations retain their matched scalar ABI.
 * The forward vectors retain the observed doubled Z store and store order.
 * g_profile_BAKUBAKU remains this file's data, not a class member.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daBakubaku_c_AssetId {
    ASSET_BAKUBAKU_BMD = 0x0295,
    ASSET_BAKUBAKU_ATTACK_BCA = 0x0296,
    ASSET_BAKUBAKU_SWIM_BCA = 0x0297
};

#include "common.h"
#include "daBakubaku_c.h"
#include "SpawnInfo.h"
#include "Player.h"
#include "SharedFilePtr.h"

bool ApproachLinear(short &value, short target, short step);

/* dCc_c bit table: flags/vulnFlags/hitFlags share it. */
enum {
    kCcCharMove = 0x2,
    kCcCharProjectile = 0x4,
    kCcMega = 0x10,
    kCcEnemy = 0x200000
};

enum { kPlayerActorId = 0xbf };

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4; the animation handles go through SharedFilePtr::Construct
 * and SharedFilePtr_Destruct_Anim. The wrappers are declared, never defined:
 * the manifest aliases their undefined members onto the ROM veneers. Both
 * paths store a u16 file ID at +0, a byte reference count at +2 and a loaded
 * file pointer at +4. */
struct BakubakuModelFilePtr : SharedFilePtr {
    u16 fileID;
    u8 numRefs;
    void *filePtr;

    BakubakuModelFilePtr(u32 fileID);
    ~BakubakuModelFilePtr();
};

struct BakubakuAnimationFileHandle : SharedFilePtr {
    u16 fileID;
    u8 numRefs;
    void *filePtr;

    BakubakuAnimationFileHandle(u32 fileID);
    ~BakubakuAnimationFileHandle();
};

/* BakuVec3 is three plain words: the retail initializer registers no
 * destructor for the cylinder offsets, so they were never Vector3 globals
 * (Vector3 carries a declared ~Vector3, which would register at exit). */
struct BakuVec3 { s32 x, y, z; };

extern "C" {
extern BakubakuModelFilePtr gBakubakuModelFile;    /* model 0x295 */
extern BakubakuAnimationFileHandle data_ov032_02113a50; /* swim anim 0x297 */
extern BakubakuAnimationFileHandle data_ov032_02113a48; /* bite anim 0x296 */
extern daBakubaku_c::State data_ov032_02113a8c; /* wander */
extern daBakubaku_c::State data_ov032_02113a9c; /* pause */
extern daBakubaku_c::State data_ov032_02113aac; /* chase */
extern daBakubaku_c::State data_ov032_02113a7c; /* surface */
extern daBakubaku_c::State data_ov032_02113abc; /* dive */
extern s32 data_0209f32c;
extern Matrix4x3 data_020a0e68;
extern int data_0209e650[];

extern BakuVec3 data_ov032_021137cc; /* body cylinder */
extern BakuVec3 data_ov032_021137d8; /* head cylinder */


int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
int AngleDiff(int a, int b);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
s16 Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
void Vec3_Asr(void *d, void *s, int sh);
int RandomIntInternal(int *seed);
unsigned short DecIfAbove0_Short(unsigned short *p);

void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_FromRotationY(void *m, s16 angY);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
void MulVec3Mat4x3(void *in, void *m, void *out);

/* local extern: retain the matched scalar ABI; native Fix12<int> calls remain pending. */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *self, void *bca, int a, int fix, unsigned int b);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 &offset,
    int radius, int height, u32 d, u32 e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, dActor_c *actor, int radius, int height, void *a, void *b);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, int f, int g, unsigned int h);
/* local extern: native Hurt takes Fix12<int> by value; retain the current
 * definition's scalar ABI while that call reconstruction remains pending. */
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(Player *p, void *v, u32 a, int f, u8 c, u8 d, u8 e);
/* Sets Player::mStateStep to 6 and stores this fish in mAttachedActor. */
int func_ov002_020c5cd8(void *a, void *self);
/* Sound::Play with bank 3. */
void func_02012694(int a, void *p);
/* Particle::System::New wrappers; they pass the tracker callbacks at
   +0x800 and +0x7f4. */
u32 func_02022c80(u32, u32, Fix12i, Fix12i, Fix12i, const void *);
u32 func_02022d00(u32, u32, Fix12i, Fix12i, Fix12i, void *);
void _Z14ApproachLinearRiii(int *p, int t, int s);
}

/* Both predicates are int-returning, as the ROM materializes the 0/1
   before testing it. dActor_c::mFlags bit 0x40000 is set while the actor
   is held in Yoshi's mouth. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == kPlayerActorId;
}

static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & 0x40000) != 0;
}

// @symbol daBakubaku_c_classInit
extern "C" daBakubaku_c *daBakubaku_c_classInit()
{
    return new daBakubaku_c();
}

// @symbol g_profile_BAKUBAKU
extern "C" SpawnInfo<daBakubaku_c> g_profile_BAKUBAKU = {
    daBakubaku_c_classInit,
    0x00e4, /* BAKUBAKU registry id 228 */
    0x0052,
    3,
    0,
    0x003e8000,
    0x01000000,
    0x01000000,
};

// @symbol _ZN12daBakubaku_c16OnAimedAtWithEggEv
int daBakubaku_c::OnAimedAtWithEgg()
{
    return 0xa0000; /* Fix12 160.0, egg aim height */
}

// @symbol _ZN12daBakubaku_c13InitResourcesEv
s32 daBakubaku_c::InitResources()
{
    Vector3 bodyOffset;
    Vector3 headOffset;
    void *f;

    f = Model::LoadFile(*(SharedFilePtr *)&gBakubakuModelFile);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov032_02113a50);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov032_02113a48);

    bodyOffset.x = data_ov032_021137cc.x;
    bodyOffset.y = data_ov032_021137cc.y;
    bodyOffset.z = data_ov032_021137cc.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mBodyClsn, this, bodyOffset, 0x64000, 0x64000,
        kCcEnemy | kCcCharProjectile, kCcMega);

    headOffset.x = data_ov032_021137d8.x;
    headOffset.y = data_ov032_021137d8.y;
    headOffset.z = data_ov032_021137d8.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mHeadClsn, this, headOffset, 0x64000, 0x8c000,
        kCcEnemy | kCcCharProjectile, 0);

    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x64000, 0, 0, 0);

    mTerminalVelocity = -0x1e000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (BCA_File *)data_ov032_02113a50.filePtr, 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;

    func_ov032_02111ff4((State *)&data_ov032_02113a8c);
    return 1;
}

// @symbol _ZN12daBakubaku_c8BehaviorEv
s32 daBakubaku_c::Behavior()
{
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3) != 0)
        return 1;

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Short(&mChaseCooldown);

    State *state = mState;
    if (state->main != 0)
        (this->*(state->main))();

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    UpdatePos(&mBodyClsn);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov032_02112044();

    if (mState != &data_ov032_02113aac) {
        mModelAnim.speed = 0x1000;
    } else {
        mModelAnim.speed = 0x2000;
    }

    mModelAnim.Advance();
    func_ov032_021113fc();
    mBodyClsn.Clear();
    mHeadClsn.Clear();

    Player *p = ClosestPlayer();
    if (p != 0 && p->mIsVanish == 0) {
        mBodyClsn.Update();
        mHeadClsn.Update();
    }

    return 1;
}

// @symbol _ZN12daBakubaku_c6RenderEv
s32 daBakubaku_c::Render()
{
    if (IsInYoshiMouth(this))
        return 1;
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN12daBakubaku_c16OnPendingDestroyEv
void daBakubaku_c::OnPendingDestroy()
{
}

// @symbol _ZN12daBakubaku_c16CleanupResourcesEv
s32 daBakubaku_c::CleanupResources()
{
    ((SharedFilePtr *)&gBakubakuModelFile)->Release();
    ((SharedFilePtr *)&data_ov032_02113a50)->Release();
    ((SharedFilePtr *)&data_ov032_02113a48)->Release();
    return 1;
}

/* Model matrix, shadow matrix, drop shadow. */
// @symbol _ZN12daBakubaku_c19func_ov032_02112044Ev
void daBakubaku_c::func_ov032_02112044()
{
    Vector3 v;
    Vec3_Asr(&v, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(
        &data_020a0e68, mAngleX, mAngleY, mAngleZ);
    mModelAnim.mat4x3 = data_020a0e68;
    Matrix4x3_FromTranslation(
        &data_020a0e68, mPosX >> 3,
        (mPosY - 0x5a000) >> 3, mPosZ >> 3);
    mShadowMat = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mShadowMat, 0xfa000, 0x258000, 0xf);
}

/* Install mState and run its enter hook. The StateFn * read-back of
   mState is the shape that keeps the store-then-load the ROM emits. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111ff4EPv
int daBakubaku_c::func_ov032_02111ff4(void *pv)
{
    State *p = (State *)pv;
    mState = p;
    StateFn *q = (StateFn *)mState;
    if (*q == 0) return 1;
    return (this->*(*q))();
}

/* Wander enter: random yaw and a state timer. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111f9cEv
int daBakubaku_c::func_ov032_02111f9c()
{
    unsigned int r = RandomIntInternal(data_0209e650);
    mAngTarget = ((r >> 8) & 0xf) << 0xc;
    r = RandomIntInternal(data_0209e650);
    mStateTimer = ((r >> 8) & 0x3f) + 0x32;
    mModelAnim.speed = 0x1000;
    return 1;
}

/* Wander: steer toward the spawn point, or start a chase. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111e24Ev
int daBakubaku_c::func_ov032_02111e24()
{
    Vector3 in;
    Vector3 out;
    short ang;

    _Z14ApproachLinearRiii(&mHorzSpeed, 0x5000, 0x333);
    if (func_ov032_02111350() == 1) {
        mStateTimer = 0x28;
        mChaseCooldown = 0x28;
        ang = Vec3_HorzAngle(&mPos, (const Vector3 *)&mSpawnPosX);
        mAngTarget = ang;
    }
    ApproachLinear(mPrevAngleY, mAngTarget, 0x100);
    ang = Vec3_VertAngle(&mPos, (const Vector3 *)&mSpawnPosX);
    ApproachLinear(mPrevAngleX, ang, 0x100);
    in.z = 0;
    in.z = 0x5000;
    in.x = 0;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    mVertSpeed = out.y;
    if (func_ov032_02111254() == 1) {
        func_ov032_02111ff4(&data_ov032_02113aac);
        return 1;
    }
    if (*(unsigned short *)&mStateTimer == 0) {
        unsigned int r = (unsigned int)RandomIntInternal(data_0209e650);
        if (((r >> 8) & 3) == 0) {
            func_ov032_02111ff4(&data_ov032_02113a9c);
        } else {
            func_ov032_02111ff4((State *)&data_ov032_02113a8c);
        }
    }
    return 1;
}

/* Pause enter: short timer, kill the velocity. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111dd8Ev
int daBakubaku_c::func_ov032_02111dd8()
{
    unsigned int r = RandomIntInternal(data_0209e650);
    mStateTimer = ((r >> 8) & 0x1f) + 0x14;
    mModelAnim.speed = 0x1000;
    unk_0a4 = 0;
    mVertSpeed = 0;
    unk_0ac = 0;
    return 1;
}

/* Pause: give up into a chase, else wander again. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111d7cEv
int daBakubaku_c::func_ov032_02111d7c()
{
    if (func_ov032_02111254() == 1) {
        func_ov032_02111ff4(&data_ov032_02113aac);
        return 1;
    }
    if (*(unsigned short *)&mStateTimer == 0)
        func_ov032_02111ff4((State *)&data_ov032_02113a8c);
    return 1;
}

/* Chase enter. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111d58Ev
int daBakubaku_c::func_ov032_02111d58()
{
    mStateTimer = 0x12c;
    mModelAnim.speed = 0x2000;
    mHorzSpeed = 0xa000;
    return 1;
}

/* Chase: close on the stored player position, then surface or dive. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111b9cEv
int daBakubaku_c::func_ov032_02111b9c()
{
    /* Out of time, chase aborted, or no target: back to wandering. */
    if (*(unsigned short *)&mStateTimer == 0 || func_ov032_02111350() == 1 ||
        func_ov032_02111254() == 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &mModelAnim, (BCA_File *)data_ov032_02113a50.filePtr, 0, 0x1000, 0);
        mChaseCooldown = 0x64;
        func_ov032_02111ff4((State *)&data_ov032_02113a8c);
        return 1;
    }

    mAngTarget = Vec3_HorzAngle(&mPos, (Vector3 *)&mTargetPosX);
    unsigned int r = (unsigned int)RandomIntInternal(data_0209e650);
    int s5 = (int)(((r >> 8) & 3) << 0x1c) >> 0x10;
    ApproachLinear(mPrevAngleY, mAngTarget, 0x200);
    if (Vec3_HorzDist(&mPos, (Vector3 *)&mTargetPosX) < 0x258000) {
        int d = AngleDiff(mAngleY, mAngTarget);
        if (d < (int)(short)(s5 + 0x200)) {
            mAngTarget = Vec3_VertAngle(&mPos, (Vector3 *)&mTargetPosX);
            mFlags = 0;
            mModelAnim.speed = 0x1000;
            mDiveStartY = mPosY;
            mStateTimer = 0;
            mHorzSpeed = 0x14000;
            unk_429 = 0;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                &mModelAnim, (BCA_File *)data_ov032_02113a48.filePtr, 0x40000000, 0x1000, 0);
            if (mAngTarget > 0)
                func_ov032_02111ff4(&data_ov032_02113a7c);
            else
                func_ov032_02111ff4(&data_ov032_02113abc);
            return 1;
        }
    }
    return 1;
}

/* Dive enter: pitch down, and let the cylinders count as char-movement. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111b50Ev
int daBakubaku_c::func_ov032_02111b50()
{
    mAngTarget = -0x4000;
    mBodyClsn.flags |= kCcCharMove;
    mHeadClsn.flags |= kCcCharMove;
    mMouthOpen = 0;
    mHorzSpeed = 0xa000;
    mLungePhase = 0;
    return 1;
}

/* Dive: splash when the mouth crosses the surface, then wander. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111830Ev
int daBakubaku_c::func_ov032_02111830()
{
    s16 speed;
    speed = 0x3000;
    if (data_0209f32c - 0x64000 > mPosY)
        speed = 0;
    if (mLungePhase == 0) {
        if (data_0209f32c > mPosY) {
            if (mAngTarget > 0)
                mLungePhase = 1;
        }
    }
    if (mLungePhase > 0 && mLungePhase < 5) {
        mLungePhase++;
        if (mLungePhase == 4) {
            Vector3 splashPosition;
            Vector3 forwardOffset;
            Vector3 rotatedOffset;
            splashPosition.x = mPosX;
            splashPosition.y = mPosY;
            splashPosition.z = mPosZ;
            forwardOffset.z = 0;
            forwardOffset.x = 0;
            forwardOffset.y = 0;
            rotatedOffset.x = 0;
            rotatedOffset.y = 0;
            rotatedOffset.z = 0;
            forwardOffset.z = 0xa0000;
            Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
            MulVec3Mat4x3(&forwardOffset, &data_020a0e68, &rotatedOffset);
            splashPosition.x += rotatedOffset.x;
            splashPosition.z += rotatedOffset.z;
            func_02022c80(0, 0x55, splashPosition.x, splashPosition.y, splashPosition.z, 0);
            mSplashParticle = func_02022d00(mSplashParticle, 0x56, splashPosition.x, data_0209f32c,
                                            splashPosition.z, 0);
            splashPosition.y += 0x4b000;
            func_02022c80(0, 0x54, splashPosition.x, splashPosition.y, splashPosition.z, 0);
        }
    }

    if (func_ov032_02111350() == 1 || func_ov032_02111254() == 0) {
        speed = 0;
        mHorzSpeed = 0;
        unk_0a4 = 0;
        mVertSpeed = 0;
        unk_0ac = 0;
    } else {
        Vector3 in2;
        Vector3 out2;
        in2.z = 0;
        in2.z = 0x14000;
        in2.x = 0;
        in2.y = 0;
        out2.x = 0;
        out2.y = 0;
        out2.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        MulVec3Mat4x3(&in2, &data_020a0e68, &out2);
        mVertSpeed = out2.y;
    }
    ApproachLinear(mAngTarget, speed, 0x200);
    ApproachLinear(mPrevAngleX, mAngTarget, 0x200);
    if (mModelAnim.Finished() != 0) {
        mMouthOpen = 0;
        if (speed == 0) {
            s16 a = mPrevAngleX;
            if (a < 0) a = -a;
            if (a < 0x100) {
                mChaseCooldown = 0x64;
                mAngTarget = mAngleY;
                mFlags = 3; /* profile clip bits 1|2 */
                mBodyClsn.flags &= ~kCcCharMove;
                mHeadClsn.flags &= ~kCcCharMove;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    &mModelAnim, (BCA_File *)data_ov032_02113a50.filePtr,
                    0, 0x1000, 0);
                func_ov032_02111ff4((State *)&data_ov032_02113a8c);
                return 1;
            }
        }
    } else {
        /* currFrame is 20.12. lsl #4 / lsr #16 is the integer frame. */
        unsigned int t = (unsigned int)(mModelAnim.currFrame << 4) >> 0x10;
        if (t > 0x14 && t < 0x3c)
            mMouthOpen = 1;
        else
            mMouthOpen = 0;
    }
    return 1;
}

/* Surface enter. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111814Ev
int daBakubaku_c::func_ov032_02111814()
{
    mLungePhase = 0;
    mMouthOpen = 0;
    mHorzSpeed = 0xa000;
    return 1;
}

/* Surface toward the player, then wander. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111620Ev
int daBakubaku_c::func_ov032_02111620()
{
    Player *r5 = ClosestPlayer();
    if (r5 == 0)
        return 1;

    if (func_ov032_02111350() == 1 || func_ov032_02111254() == 0) {
        mHorzSpeed = 0;
        unk_0a4 = 0;
        mVertSpeed = 0;
        unk_0ac = 0;
    } else {
        Vector3 in;
        Vector3 out;
        in.z = 0;
        in.z = 0x14000;
        in.x = 0;
        in.y = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        mVertSpeed = out.y;
    }

    /* Once the lunge has started or the bite animation is over, level out
       and go back to wandering; until then, pitch toward the player. */
    if (mLungePhase != 0 || mModelAnim.Finished() != 0) {
        ApproachLinear(mPrevAngleX, 0, 0x200);
        mLungePhase = 1;
        mMouthOpen = 0;
        if (AngleDiff(mPrevAngleX, 0) < 0x200) {
            mPrevAngleX = 0;
            mChaseCooldown = 0x64;
            mAngTarget = mAngleY;
            mFlags = 3;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                &mModelAnim, (BCA_File *)data_ov032_02113a50.filePtr, 0, 0x1000, 0);
            func_ov032_02111ff4((State *)&data_ov032_02113a8c);
        }
    } else {
        Vector3 vec;
        unsigned int n;
        const s32 *playerPos = &r5->mPosX;
        vec.x = playerPos[0];
        vec.y = playerPos[1];
        vec.z = playerPos[2];
        if (data_0209f32c > mPosY) {
            ApproachLinear(mPrevAngleX, 0, 0x200);
        } else {
            ApproachLinear(mPrevAngleX, Vec3_VertAngle(&mPos, &vec), 0x200);
        }
        n = (unsigned int)(mModelAnim.currFrame << 4) >> 0x10;
        if (n > 0x14 && n < 0x3c)
            mMouthOpen = 1;
        else
            mMouthOpen = 0;
    }
    return 1;
}

/* Body cylinder: mega-kill or a bite. Head cylinder: mega-kill or Hurt. */
// @symbol _ZN12daBakubaku_c19func_ov032_021113fcEv
void daBakubaku_c::func_ov032_021113fc()
{
    Vector3 v1;
    v1.x = data_ov032_021137cc.x;
    v1.y = data_ov032_021137cc.y;
    v1.z = data_ov032_021137cc.z;
    mBodyClsn.SetPosRelativeToActor(v1);

    u32 id1 = mBodyClsn.otherOwner;
    if (id1 != 0) {
        dActor_c *actor = dActor_c::FindWithID(id1);
        if (IsPlayerActor(actor)) {
            Player *f = static_cast<Player *>(actor);
            if (f->mIsVanish != 0) return;
            if (mBodyClsn.hitFlags & kCcMega) {
                SpawnMegaCharParticles(*f, 0);
                PoofDust();
                f->IncMegaKillCount();
                func_02012694(0x1e, &mCamSpacePosX);
                KillAndTrackInDeathTable();
                return;
            }
            if (mMouthOpen != 0) {
                if (func_ov002_020c5cd8(f, this) == 1) {
                    func_02012694(0xf6, &mCamSpacePosX);
                    return;
                }
            }
        }
    }

    Vector3 v2;
    v2.x = data_ov032_021137d8.x;
    v2.y = data_ov032_021137d8.y;
    v2.z = data_ov032_021137d8.z;
    mHeadClsn.SetPosRelativeToActor(v2);

    u32 id2 = mHeadClsn.otherOwner;
    if (id2 == 0) return;
    dActor_c *actor2 = dActor_c::FindWithID(id2);
    if (!IsPlayerActor(actor2))
        return;
    Player *f2 = static_cast<Player *>(actor2);

    if (mHeadClsn.hitFlags & kCcMega) {
        SpawnMegaCharParticles(*f2, 0);
        PoofDust();
        f2->IncMegaKillCount();
        KillAndTrackInDeathTable();
        return;
    }

    if (mState == &data_ov032_02113abc) return;
    if (mState == &data_ov032_02113a7c) return;

    Vector3 hv;
    hv.x = mPosX;
    hv.y = mPosY;
    hv.z = mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(f2, &hv, 2, 0xc000, 1, 0, 1);
}

/* Abort the chase: no player, a wall or floor, or too far from home.
   Above the water aborts too, except during the dive. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111350Ev
int daBakubaku_c::func_ov032_02111350()
{
    if (ClosestPlayer() == 0) return 1;
    if (mWithMeshClsn.IsOnWall() != 0
        || mWithMeshClsn.IsOnGround() != 0)
        return 1;
    if (Vec3_HorzDist((struct Vector3 *)&mSpawnPosX, &mPos) > 0x4b0000)
        return 1;
    if (mState != &data_ov032_02113abc) {
        if (data_0209f32c < mPosY) return 1;
    }
    return 0;
}

/* Mario is a chase target. Stores his position. */
// @symbol _ZN12daBakubaku_c19func_ov032_02111254Ev
int daBakubaku_c::func_ov032_02111254()
{
    Player *pl = ClosestPlayer();
    int *s;
    void *t;
    int d;
    if (pl == 0 || mChaseCooldown != 0)
        return 0;
    s = (int *)&pl->mPosX;
    mTargetPosX = s[0];
    mTargetPosY = s[1];
    mTargetPosZ = s[2];
    t = mState;
    if (t != &data_ov032_02113abc && t != &data_ov032_02113a7c) {
        if (pl->mIsUnderwater == 0)
            return 0;
    }
    d = pl->mGroundY - data_0209f32c;
    if (d < 0) d = -d;
    if (d < 0xb4000)
        return 0;
    if (t != &data_ov032_02113abc && t != &data_ov032_02113a7c) {
        if (Vec3_HorzDist(
                (const Vector3 *)&mSpawnPosX,
                (const Vector3 *)&mTargetPosX) > 0x4b0000)
            return 0;
    }
    return 1;
}

/* Retail initializer order is model 0x295, swim anim 0x297, bite anim
 * 0x296, then the wander/pause/chase/dive/surface State records. mwcc
 * emits __sinit_d_a_bakubaku.cpp from these definitions, including the
 * destructor registrations and the PMF descriptor constants at
 * 0x0211377c..0x021137c4. */
BakubakuModelFilePtr gBakubakuModelFile(ASSET_BAKUBAKU_BMD);
BakubakuAnimationFileHandle data_ov032_02113a50(ASSET_BAKUBAKU_SWIM_BCA);
BakubakuAnimationFileHandle data_ov032_02113a48(ASSET_BAKUBAKU_ATTACK_BCA);
daBakubaku_c::State data_ov032_02113a8c = {
    &daBakubaku_c::func_ov032_02111f9c, &daBakubaku_c::func_ov032_02111e24
};
daBakubaku_c::State data_ov032_02113a9c = {
    &daBakubaku_c::func_ov032_02111dd8, &daBakubaku_c::func_ov032_02111d7c
};
daBakubaku_c::State data_ov032_02113aac = {
    &daBakubaku_c::func_ov032_02111d58, &daBakubaku_c::func_ov032_02111b9c
};
daBakubaku_c::State data_ov032_02113abc = {
    &daBakubaku_c::func_ov032_02111b50, &daBakubaku_c::func_ov032_02111830
};
daBakubaku_c::State data_ov032_02113a7c = {
    &daBakubaku_c::func_ov032_02111814, &daBakubaku_c::func_ov032_02111620
};
BakuVec3 data_ov032_021137cc = { 0, -0x28000, 0x58000 };   /* body cylinder */
BakuVec3 data_ov032_021137d8 = { 0, -0x30000, -0x20000 };  /* head cylinder */
