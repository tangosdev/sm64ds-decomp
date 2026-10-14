//cpp
/* Dorrie (DOSSY) and the cap she wears (DOSSY_CAP), plus the platform
 * element of mPlatforms. ov065 0x02117f40..0x021196d8, 29 functions.
 *
 * _ZTS9daDossy_c 0x0211cd34, _ZTS12daDossyCap_c 0x0211cd58, each naming
 * _ZTI8dActor_c. _ZTV9daDossy_c 0x0211ce48, _ZTV12daDossyCap_c 0x0211cdc4.
 * Both destructors are inline, so each D1/D0 pair lands at the bottom of
 * the run and InitResources is the key function. Highest address first.
 * opt_common_subs off and opt_strength_reduction off are file-wide:
 * func_ov065_02118838 needs both, and a bracket around that function
 * does not bind.
 */
// leftovers:
// - ModelAnim::SetAnim and the other Fix12<int>-by-value calls stay
//   mangled. A member SetAnim with Fix12<int> speed in func_ov065_021182e4
//   changes that function.
// - func_ov065_021180d4, func_ov065_02118c4c and func_ov065_02119210 take
//   daDossyCap_c, so they stay free functions. decl_common.h still
//   declares those forms.
// - func_ov065_02118838 keeps `(i << 9)` platform addressing. A typed
//   DorriePlatform walk, or mPlatforms[i], changes that function and the
//   char* walks in CleanupResources and InitResources. Casting
//   `c + (i << 9) + 0x150` to DorriePlatform * is 4 bytes longer, and
//   `(char *)mPlatforms + (i << 9)` is 12 bytes longer.
// - The seven parameter names on the prototypes of Matrix4x3_FromRotationY,
//   DecIfAbove0_Short and the two mangled method externs hold the @NNN
//   count: removing the gotos from func_ov065_021183c8 and
//   func_ov065_021180d4 dropped it by 7, and these 7 names put it back.
// - daDossyCap_c::Render tests the cap-icon flag as `(flags << 30) >> 31`.
//   `>> 1 & 1` selects asr/ands instead of lsl/lsrs.
// - data_ov065_* keep their address names, including the state table
//   data_ov065_0211d7fc.
// - func_ov065_021195bc and func_ov065_021195d0 are veneers. The first
//   argument is not the actor. An explicit cast of &daDossy_c::method to
//   a function pointer is rejected, so each veneer calls the method
//   through its mangled name with the second and third arguments.


#pragma opt_common_subs off
#pragma opt_strength_reduction off
#include "common.h"
#include "daDossy_c.h"
#include "daDossyCap_c.h"
#include "dBgW.h"
#include "SharedFilePtr.h"
#include "Player.h"

typedef enum { FALSE = 0, TRUE = 1 } BOOL;

/* The resource handles as the ROM lays them out: {id, loaded file}. The
   model handle constructs through func_02017acc and destroys through
   func_02017ab4; the animation handles construct through
   SharedFilePtr::Construct and destroy through SharedFilePtr_Destruct_Anim;
   the collision handles construct through func_02017b4c and destroy through
   SharedFilePtr_Destruct_Clsn. The manifest aliases each undefined member
   onto its ROM symbol. */
struct DossyModelFilePtr : SharedFilePtr {
    u32 id; void *ptr;

    DossyModelFilePtr(u32 fileID);
    ~DossyModelFilePtr();
};

struct DossyAnimationFileHandle : SharedFilePtr {
    u32 id; void *ptr;

    DossyAnimationFileHandle(u32 fileID);
    ~DossyAnimationFileHandle();
};

struct DossyCollisionFileHandle : SharedFilePtr {
    u32 id; void *ptr;

    DossyCollisionFileHandle(u32 fileID);
    ~DossyCollisionFileHandle();
};

extern DossyModelFilePtr data_ov065_0211d720;
extern DossyAnimationFileHandle data_ov065_0211d768;
extern DossyAnimationFileHandle data_ov065_0211d748;
extern DossyAnimationFileHandle data_ov065_0211d770;
extern DossyCollisionFileHandle data_ov065_0211d738;
extern DossyCollisionFileHandle data_ov065_0211d750;
extern DossyCollisionFileHandle data_ov065_0211d728;
extern DossyCollisionFileHandle data_ov065_0211d760;
extern DossyCollisionFileHandle data_ov065_0211d758;
extern DossyCollisionFileHandle data_ov065_0211d740;
extern DossyCollisionFileHandle data_ov065_0211d730;

extern "C" {
extern s16 data_02082214[];
extern Matrix4x3 data_020a0e68;
extern u8 data_ov065_0211c078[];
extern void *data_ov065_0211cd68[];
extern SharedFilePtr data_ov002_0210d9c0;
extern SharedFilePtr *data_ov065_0211c080[];
extern SharedFilePtr *data_ov065_0211c08c[];

extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int n, int fix, u32 flags);
extern void func_0201267c(u32 id, char* p);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
void Matrix4x3_FromTranslation(Matrix4x3 *m, s32 x, s32 y, s32 z);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *m, s16 a);
void MulMat4x3Mat4x3(const Matrix4x3 *a, const Matrix4x3 *b, Matrix4x3 *o);
void SubVec3(const Vector3 *a, const Vector3 *b, Vector3 *o);
void AddVec3(const Vector3 *a, const Vector3 *b, Vector3 *o);
void Vec3_LslInPlace(Vector3 *v, s32 sh);
extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, Fix12i r, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void func_020393c4(void* p, void* v);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, void* a, Fix12i r, Fix12i h, void* p, void* q);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* a, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* self, void* a, void* pos, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
extern int Vec3_HorzDist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
extern int AngleDiff(int a, int b);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
extern u16 DecIfAbove0_Short(void *counter);

int func_ov065_021180d4(char* self);
void func_ov065_02118c4c(char* c);
int func_ov065_02119210(unsigned char* c);
int func_ov065_021195bc(void *a, void *b, void *c);
int func_ov065_021195d0(void *a, void *b, void *c);
/* local extern: mwcc rejects an explicit cast of &daDossy_c::method to a function pointer. */
extern void _ZN9daDossy_c19func_ov065_0211956cEP8dActor_c(void *self, void *other); /* local extern: veneer calls this method by its mangled name. */
extern void _ZN9daDossy_c19func_ov065_02119594EP8dActor_c(void *self, void *other); /* local extern: veneer calls this method by its mangled name. */
}

int ApproachLinear(int &value, int target, int step);
bool ApproachLinear(short &value, short target, short step);

/* The loaded file behind a SharedFilePtr is its second word. */
#define LOADED_FILE(ptr) ((void *)((int *)&(ptr))[1])

typedef void (daDossy_c::*StateFunc)();
extern "C" StateFunc data_ov065_0211d7fc[];

// @symbol _ZN14DorriePlatformC1Ev
DorriePlatform::DorriePlatform()
{
}

// @symbol daDossy_c_classInit
extern "C" daDossy_c *daDossy_c_classInit(void)
{
    return new daDossy_c();
}

// @symbol daDossyCap_c_classInit
extern "C" daDossyCap_c *daDossyCap_c_classInit(void)
{
    return new daDossyCap_c();
}

// @symbol _ZN12daDossyCap_c13OnYoshiTryEatEv
int daDossyCap_c::OnYoshiTryEat()
{
    return 4;
}

#pragma long_calls on
// @symbol func_ov065_021195d0
extern "C" int func_ov065_021195d0(void *a, void *b, void *c)
{
    return ((int (*)(void *, void *))_ZN9daDossy_c19func_ov065_02119594EP8dActor_c)(b, c);
}
#pragma long_calls off

#pragma long_calls on
// @symbol func_ov065_021195bc
extern "C" int func_ov065_021195bc(void *a, void *b, void *c)
{
    return ((int (*)(void *, void *))_ZN9daDossy_c19func_ov065_0211956cEP8dActor_c)(b, c);
}
#pragma long_calls off

// @symbol _ZN9daDossy_c19func_ov065_02119594EP8dActor_c
void daDossy_c::func_ov065_02119594(dActor_c *other)
{
    BOOL isPlayer = (other->actorID == 0xbf) ? TRUE : FALSE;
    if (isPlayer) {
        mClsnState = 1;
        mClsnPlayer = other;
    }
}

// @symbol _ZN9daDossy_c19func_ov065_0211956cEP8dActor_c
void daDossy_c::func_ov065_0211956c(dActor_c *other)
{
    BOOL isPlayer = (other->actorID == 0xbf) ? TRUE : FALSE;
    if (isPlayer) {
        mClsnState = 2;
        mClsnPlayer = other;
    }
}

// @symbol _ZN12daDossyCap_c13InitResourcesEv
int daDossyCap_c::InitResources()
{
    mModel.SetFile((BMD_File *)LOADED_FILE(data_ov002_0210d9c0), 1, -1);
    mCapIcon.func_ov001_020ab228((char *)this, 2, 2, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x96000, 0x96000, 0x100002, 0x8000);
    mCarrier = 0;
    return 1;
}

// @symbol _ZN9daDossy_c13InitResourcesEv
int daDossy_c::InitResources()
{
    int i;
    void* f = Model::LoadFile(data_ov065_0211d720);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);
    Model::LoadFile(data_ov002_0210d9c0);
    for (i = 0; i < 3; i++)
        dExtFrameCtrl_c::LoadFile(*data_ov065_0211c080[i]);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov065_0211d770.ptr, 0, 0x1000, 0);
    func_ov065_02118cc4();
    func_ov065_02118838();

    {
        int j;
        char* mtx = (char *)&mPlatforms[0].mClsnNextMat;
        char* mmc = (char *)&mPlatforms[0].mClsn;
        for (j = 0; j < 7; j++) {
            KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov065_0211c08c[j]);
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                mmc, kcl, mtx, 0x1000, mAngleY, data_ov065_0211cd68[j]);
            if ((unsigned)j <= 2) {
                func_020393d4(mmc, (void *)&dBgW::UpdatePosAndAngs);
                if (j == 2)
                    func_020393c4(mmc, (void *)func_ov065_021195bc);
                else
                    func_020393c4(mmc, (void *)func_ov065_021195d0);
            }
            ((dBgW_KcMbg *)mmc)->Enable(this);
            mtx += sizeof(DorriePlatform);
            mmc += sizeof(DorriePlatform);
        }
    }

    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mPosX += 0x7d0000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x1e0000, 0xa0000, 0, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylClsn1, this, 0xdc000, 0xfa000, 2, 0x20);
    {
        int v[3];
        v[0] = 0;
        v[1] = 0x50000;
        v[2] = 0x150000;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            &mCylClsn2, this, v, 0x60000, 0x1b0000, 2, 0x400000);
    }
    {
        mHomePosX = mPosX;
        mHomePosY = mPosY;
        mHomePosZ = mPosZ;
        mStateTimer = 0;
        mClsnState = 0;
        mClsnPlayer = 0;
        mRider = 0;
        mSinkHeight = 0;
        mPushDownHeight = mSinkHeight;
    }
    if ((param1 & 0xff) == 1)
        mHasCap = 1;
    else
        mHasCap = 0;
    mCap = 0;
    if (mHasCap != 0) {
        mCapPosX = 0;
        mCapPosY = 0;
        mCapPosZ = 0;
        func_ov065_02118838();
        {
            daDossyCap_c *cap = (daDossyCap_c *)dActor_c::Spawn(
                0xa9, 0, *(Vector3 *)&mPosX, (Vector3_16 *)&mAngleX, mAreaId, -1);
            if (cap != 0) {
                mCap = cap;
                cap->mCarrier = (s32)this;
            } else {
                mHasCap = 0;
            }
        }
    }
    return 1;
}

// @symbol func_ov065_02119210
extern "C" int func_ov065_02119210(unsigned char *c)
{
    daDossyCap_c *cap = (daDossyCap_c *)c;
    int flags = cap->mCapIcon.mFlags << 30;
    return ((unsigned)flags >> 31) != 0;
}

// @symbol _ZN12daDossyCap_c8BehaviorEv
int daDossyCap_c::Behavior()
{
    daDossy_c *carrier = (daDossy_c *)mCarrier;
    if (carrier == 0)
        return 1;
    int eaten = (mFlags & 0x20000) != 0;
    if (eaten != 0) {
        dActor_c *spit = dActor_c::Spawn(0x10d, 0x1210, *(Vector3 *)&mPosX,
                                         (Vector3_16 *)&mAngleX, mAreaId, -1);
        if (spit != 0) {
            /* dEnemyBase_c::mEatingPlayer of the spawned daObjMarioCap_c;
               including dEnemyBase_c.h renumbers the pins by 26. */
            *(s32 *)((char *)spit + 0xd0) = mEatingPlayer;
            ((Player *)mEatingPlayer)->mObjInMouth = (s32)spit;
            spit->mFlags |= 0x20000;
            mFlags &= ~0x20000;
            mEatingPlayer = 0;
            mCapIcon.Unlink();
            mCapIcon.func_ov001_020ab228((char *)this, 2, 0, 0);
        }
        return 1;
    }
    s32 *capPos = &carrier->mCapPosX;
    mPosX = capPos[0];
    mPosY = capPos[1];
    mPosZ = capPos[2];
    mAngleX = ((daDossy_c *)mCarrier)->mHeadRotX;
    mAngleY = ((daDossy_c *)mCarrier)->mAngleY;
    unsigned int flags = mCapIcon.mFlags;
    if ((flags << 30) >> 31) {
        func_ov065_02118c4c((char *)this);
        if (func_ov065_021180d4((char *)this) != 0) {
            mdCcAc_c.Clear();
            return 1;
        }
        mdCcAc_c.Clear();
        mdCcAc_c.Update();
    }
    return 1;
}

// @symbol _ZN9daDossy_c8BehaviorEv
int daDossy_c::Behavior()
{
    int d;
    int outer;
    int inner;

    if (mClsnState != 0)
        ApproachLinear(mSinkHeight, 0xa000, 0x1000);
    else
        ApproachLinear(mSinkHeight, 0, 0x1000);

    (this->*data_ov065_0211d7fc[mState])();

    mDistToCenter = Vec3_HorzDist(&mPosX, &mSpawnPosX);
    mAngToCenter = Vec3_HorzAngle(&mPosX, &mSpawnPosX);

    d = (short)AngleDiff(mAngToCenter, mAngleY);

    {
        short sv = data_02082214[((unsigned short)d >> 4) * 2 + 1];
        short cv = data_02082214[((unsigned short)mPlatforms[1].mRot.x >> 4) * 2];
        int sm = sv * 0x190;
        int t = (int)(((long long)sm * cv + 0x800) >> 12);
        if (d < 0x4000) {
            inner = t + 0x5f8000;
            outer = 0x97c000;
        } else {
            inner = 0x5f8000;
            outer = t + 0x97c000;
        }
    }

    if (mDistToCenter >= outer) {
        mPosX = mSpawnPosX
            - (int)(((long long)outer * data_02082214[((unsigned short)mAngToCenter >> 4) * 2] + 0x800) >> 12);
        mPosZ = mSpawnPosZ
            - (int)(((long long)outer * data_02082214[((unsigned short)mAngToCenter >> 4) * 2 + 1] + 0x800) >> 12);
    } else if (mDistToCenter <= inner) {
        mPosX = mSpawnPosX
            - (int)(((long long)inner * data_02082214[((unsigned short)mAngToCenter >> 4) * 2] + 0x800) >> 12);
        mPosZ = mSpawnPosZ
            - (int)(((long long)inner * data_02082214[((unsigned short)mAngToCenter >> 4) * 2 + 1] + 0x800) >> 12);
    }

    UpdatePos(0);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    mPosY = mSpawnPosY - mSinkHeight - mPushDownHeight;
    mModelAnim.Advance();
    func_ov065_02118cc4();
    func_ov065_02118838();
    func_ov065_02118248();
    mClsnState = 0;
    mClsnPlayer = 0;
    return 1;
}

// @symbol _ZN12daDossyCap_c6RenderEv
int daDossyCap_c::Render()
{
    unsigned int flags = mCapIcon.mFlags;
    if ((flags << 30) >> 31) {
        Vector3 scale;
        scale.x = 0x2c00;
        scale.y = 0x2c00;
        scale.z = 0x2c00;
        mModel.Render(&scale);
    }
    return 1;
}

// @symbol _ZN9daDossy_c6RenderEv
int daDossy_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN9daDossy_c16CleanupResourcesEv
int daDossy_c::CleanupResources()
{
    int i;
    char *clsn;
    data_ov002_0210d9c0.Release();
    clsn = (char *)&mPlatforms[0].mClsn;
    for (i = 0; i < 7; i++) {
        ((dBgW_KcMbg *)clsn)->Disable();
        data_ov065_0211c08c[i]->Release();
        clsn += sizeof(DorriePlatform);
    }
    for (i = 0; i < 3; i++)
        data_ov065_0211c080[i]->Release();
    data_ov065_0211d720.Release();
    return 1;
}

// @symbol _ZN9daDossy_c19func_ov065_02118cc4Ev
void daDossy_c::func_ov065_02118cc4()
{
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.m[9] = mPosX >> 3;
    mModelAnim.mat4x3.m[10] = mPosY >> 3;
    mModelAnim.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol func_ov065_02118c4c
extern "C" void func_ov065_02118c4c(char *c)
{
    daDossyCap_c *cap = (daDossyCap_c *)c;
    Vector3 pos;
    Vec3_Asr(&pos, &cap->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos.x, pos.y, pos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, cap->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, cap->mAngleX);
    cap->mModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN9daDossy_c19func_ov065_02118838Ev
void daDossy_c::func_ov065_02118838()
{
    char *c = (char *)this;
    char *pm;
    char *pk;
    s32 zero;
    s32 three;
    Vector3s rot;
    Vector3 v;
    Matrix4x3 base;
    s32 i;
    u8 *tbl;

    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    base = data_020a0e68;
    pm = (char *)&mPlatforms[0].mClsnNextMat;
    pk = (char *)&mPlatforms[0].mClsn;
    zero = 0;
    three = 3;
    tbl = data_ov065_0211c078;
    for (i = 0; i < 7; i++) {
        /* data.bones holds 0x34-byte runtime pose records filled by
           UpdateBones, not BMD_Bone (0x40) as the header types it; the
           rotation sits at +0x1a/+0x1c/+0x1e. */
        char *ent = (char *)mModelAnim.data.bones + *tbl * 0x34;
        {
            u16 tz = *(u16 *)(ent + 0x1e);
            u16 ty = *(u16 *)(ent + 0x1c);
            u16 tx = *(u16 *)(ent + 0x1a);
            rot.x = tx;
            rot.y = ty;
            rot.z = tz;
        }
        *(Vector3s *)(c + (i << 9) + 0x348) = rot;
        if (i == 2) {
            *(s16 *)(c + (i << 9) + 0x348) += mPlatforms[1].mRot.x;
            *(s16 *)(c + (i << 9) + 0x34a) += mPlatforms[1].mRot.y;
            *(s16 *)(c + (i << 9) + 0x34c) += mPlatforms[1].mRot.z;
        }
        v.x = zero;
        v.y = zero;
        v.z = zero;
        {
            Matrix4x3 *m = &data_020a0e68;
            *m = base;
            MulMat4x3Mat4x3(&mModelAnim.data.transforms[*tbl], m, m);
            v.x = m->m[9];
            v.y = m->m[10];
            v.z = m->m[11];
        }
        SubVec3(&v, (Vector3 *)&mPosX, &v);
        Vec3_LslInPlace(&v, three);
        AddVec3(&v, (Vector3 *)&mPosX, &v);
        if (i == 2) {
            s32 *cx = &mCapPosX;
            s32 *cy = &mCapPosY;
            s32 *cz = &mCapPosZ;
            s32 *hx = &mHomePosX;
            s32 *hy = &mHomePosY;
            s32 *hz = &mHomePosZ;
            s32 k1e = 0x1e;
            s32 k82 = 0x82;
            s32 k96 = 0x96;
            s32 rnd = 0x800;
            s32 scaled;
            mHeadRotX = mPlatforms[2].mRot.x;
            mHomePosX = v.x;
            mHomePosY = v.y;
            mHomePosZ = v.z;
            scaled = data_02082214[((u16)mHeadRotX >> 4) * 2] * k96;
            mCapPosX = mHomePosX;
            mCapPosY = mHomePosY;
            mCapPosZ = mHomePosZ;
            *cx += (s32)(((s64)scaled * data_02082214[((u16)mAngleY >> 4) * 2] + rnd) >> 12);
            *cy += 0x8c000 - data_02082214[((u16)mHeadRotX >> 4) * 2] * k1e;
            *cz += (s32)(((s64)scaled * data_02082214[((u16)mAngleY >> 4) * 2 + 1] + rnd) >> 12);
            *hx += data_02082214[((u16)mAngleY >> 4) * 2] * k82;
            *hy += 0x50000;
            *hz += data_02082214[((u16)mAngleY >> 4) * 2 + 1] * k82;
        }
        Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, (s16)(mAngleY + *(s16 *)(c + (i << 9) + 0x34a)));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(c + (i << 9) + 0x348));
        Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16 *)(c + (i << 9) + 0x34c));
        *(Matrix4x3 *)(c + (i << 9) + 0x150) = data_020a0e68;
        ((dBgW_KcMbg *)pk)->Transform(*(Matrix4x3 *)pm, mAngleY);
        pm += 0x200;
        pk += 0x200;
        tbl++;
    }
}

// @symbol _ZN9daDossy_c19func_ov065_02118634Ev
void daDossy_c::func_ov065_02118634()
{
    int landed = 0;

    {
        u32 id = mCylClsn2.otherOwner;
        if (id != 0) {
            Player *player = (Player *)dActor_c::FindWithID(id);
            if (player != 0) {
                if (player->mIsAirborne == 0) landed = 1;
            }
        }
    }

    if (mCylClsn1.otherOwner != 0 || landed != 0) {
        mState++;
        mHorzSpeed = 0;
        mAngVelY = 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov065_0211d768.ptr, 0x40000000, 0x1000, 0);
        mCylClsn1.Clear();
        mCylClsn2.Clear();
        mStateTimer = 0;
        mStateState = 0;
        mPushDownHeight = mStateState;
        func_0201267c(0xe6, (char *)&mCamSpacePosX);
    } else {
        s16 target;
        int speed;
        dActor_c *player = mClsnPlayer;
        if (player != 0) {
            target = player->mAngleY;
            speed = 0x7000;
        } else {
            int a = _ZN4cstd5atan2E5Fix12IiES1_(0x7d0000, mDistToCenter - 0x7d0000);
            s16 base = mAngToCenter;
            s16 diff = (s16)(mAngleY - base);
            if (diff < 0) a = (s16)(-a);
            target = (s16)(base + a);
            speed = 0x3800;
        }
        ApproachLinear(mHorzSpeed, speed, 0x400);

        {
            int d = (s16)(target - mAngleY);
            ApproachLinear(mAngVelY, (s16)(d / 100), 5);
        }

        mAngleY += mAngVelY;
        mPrevAngleY = mAngleY;

        mCylClsn1.Clear();
        mCylClsn2.Clear();
        mCylClsn1.Update();
        {
            Vector3 offset;
            offset.x = 0;
            offset.y = 0x50000;
            offset.z = 0x150000;
            mCylClsn2.SetPosRelativeToActor(offset);
        }
        mCylClsn2.Update();
    }
}

// @symbol _ZN9daDossy_c19func_ov065_021183c8Ev
void daDossy_c::func_ov065_021183c8()
{
    Vector3 playerPos;
    u8 st = mStateState;

    if (st == 0) {
        mPushDownHeight += 0xa000;
        if (mPushDownHeight >= 0x64000) {
            mPushDownHeight = 0x64000;
            mStateState++;
        }
    } else if (st == 1) {
        mPushDownHeight -= 0x5000;
        if (mPushDownHeight <= 0) {
            mPushDownHeight = 0;
            mStateState++;
        }
    }

    if (mModelAnim.Finished() == 0)
        return;

    mStateTimer++;
    DecIfAbove0_Short(&mUnkTimer);

    if (mClsnPlayer != 0) {
        if (mClsnState == 2 && mUnkTimer == 0) {
            if (mCap == 0 || func_ov065_02119210((unsigned char *)mCap) == 0) {
                playerPos = *(Vector3 *)&mClsnPlayer->mPosX;
                if (playerPos.y >= mHomePosY
                    && Vec3_HorzDist(&playerPos, &mHomePosX) < 0x88000
                    && ((Player *)mClsnPlayer)->SetNoControlState(5, -1, 0) != 0) {
                    mState++;
                    mStateState = 0;
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim,
                        data_ov065_0211d748.ptr, 0x40000000, 0x1000, 0);
                    mRider = mClsnPlayer;
                }
            }
        }
        if (Vec3_HorzDist(&mPosX, &mClsnPlayer->mPosX) < 0x1f4000)
            mStateTimer = 0;
    } else {
        if (mStateTimer > 0x96) {
            mState++;
            mStateState = 0;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim,
                data_ov065_0211d748.ptr, 0x40000000, 0x1000, 0);
        }
        mStateTimer++;
    }
}

// @symbol _ZN9daDossy_c19func_ov065_021182e4Ev
void daDossy_c::func_ov065_021182e4()
{
    switch (mStateState) {
    case 0:
        if (mModelAnim.Finished() == 0) return;
        mStateState = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov065_0211d770.ptr, 0, 0x1000, 0);
        if (mRider == 0) return;
        ((Player *)mRider)->Unk_020ca150(5);
        return;
    case 1:
        if (mRider != 0) {
            if (((Player *)mRider)->Unk_020c9e5c(5) != 0) return;
        }
        mState = 0;
        mStateState = 0;
        mRider = 0;
        return;
    }
}

// @symbol _ZN9daDossy_c19func_ov065_02118248Ev
int daDossy_c::func_ov065_02118248()
{
    int hidden = (int)((mFlags & 8) != 0);
    if (hidden != 0) {
        int i;
        char *clsn = (char *)&mPlatforms[0].mClsn;
        for (i = 0; i < 7; i++) {
            if (((dBgW_KcMbg *)clsn)->IsEnabled())
                ((dBgW_KcMbg *)clsn)->Disable();
            clsn += sizeof(DorriePlatform);
        }
        return 1;
    }
    char *clsn = (char *)&mPlatforms[0].mClsn;
    int i;
    for (i = 0; i < 7; i++) {
        if (!((dBgW_KcMbg *)clsn)->IsEnabled())
            ((dBgW_KcMbg *)clsn)->Enable(this);
        clsn += sizeof(DorriePlatform);
    }
    return 0;
}

// @symbol func_ov065_021180d4
extern "C" int func_ov065_021180d4(char *self)
{
    daDossyCap_c *cap = (daDossyCap_c *)self;
    Player *player;
    u32 id;

    /* Single exit for the failure path: an early return per check is
       56 bytes longer. */
    id = cap->mdCcAc_c.otherOwner;
    if (id != 0 && (cap->mdCcAc_c.hitFlags & 0x400000)) {
        player = (Player *)dActor_c::FindWithID(id);
        if (player != 0) {
            int isPlayer = (int)(player->actorID == 0xbf);
            if (isPlayer != 0 && player->mIsAirborne == 0
                && player->Unk_020c9e5c(5) == 0) {
                /* The inner block holds the @NNN count: without it the
                   pins move up by 2. */
                {
                    Vector3 pos;
                    Vector3 *playerPos;
                    int idx;
                    int speed;

                    playerPos = (Vector3 *)&player->mPosX;
                    pos.x = playerPos->x;
                    pos.y = playerPos->y;
                    pos.z = playerPos->z;

                    idx = (u16)player->mAngleY >> 4;
                    speed = player->mHorzSpeed;

                    pos.y += 0x50000;
                    pos.x += (int)(((s64)speed *
                                       data_02082214[idx * 2] + 0x800) >> 0xc);
                    pos.z += (int)(((s64)speed *
                                       data_02082214[idx * 2 + 1] + 0x800) >> 0xc);

                    if (dActor_c::Spawn(0x10d, 0x1210, pos, (Vector3_16 *)&cap->mAngleX,
                                        cap->mAreaId, -1) != 0)
                    {
                        cap->mCapIcon.Unlink();
                        cap->mCapIcon.func_ov001_020ab228(self, 2, 0, 0);
                        ((daDossy_c *)cap->mCarrier)->mUnkTimer = 0x1e;
                    }
                }
                return 1;
            }
        }
    }
    return 0;
}

// @symbol _ZN14DorriePlatformD1Ev
DorriePlatform::~DorriePlatform()
{
}

/* The eleven file-scope resource handles, in retail initializer order: the
   model file (func_02017acc / func_02017ab4), three animation files
   (SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim), then seven
   collision files (func_02017b4c / SharedFilePtr_Destruct_Clsn). The
   manifest aliases each wrapper ctor/dtor onto those ROM symbols. */
DossyModelFilePtr data_ov065_0211d720(0x2e9);
DossyAnimationFileHandle data_ov065_0211d768(0x2ee);
DossyAnimationFileHandle data_ov065_0211d748(0x2ef);
DossyAnimationFileHandle data_ov065_0211d770(0x2f3);
DossyCollisionFileHandle data_ov065_0211d738(0x2ea);
DossyCollisionFileHandle data_ov065_0211d750(0x2eb);
DossyCollisionFileHandle data_ov065_0211d728(0x2f0);
DossyCollisionFileHandle data_ov065_0211d760(0x2f1);
DossyCollisionFileHandle data_ov065_0211d758(0x2f2);
DossyCollisionFileHandle data_ov065_0211d740(0x2ec);
DossyCollisionFileHandle data_ov065_0211d730(0x2ed);

/* The state table Behavior dispatches through: three pointer-to-member
   records. Retail stores the records as compiler temporaries in .data
   (0x0211cd2c, 0x0211cd1c, 0x0211cd24) and the live table in .bss. */
extern "C" StateFunc data_ov065_0211d7fc[3] = {
    &daDossy_c::func_ov065_02118634,
    &daDossy_c::func_ov065_021183c8,
    &daDossy_c::func_ov065_021182e4,
};
