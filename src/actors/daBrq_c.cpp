//cpp
/* Genuine production translation unit for ov070/daBrq_c.
 *
 * daBrq_c_classInit and g_profile_BIRIKYU are reconstructed source-style
 * names. SM64DS proves the daBrq_c RTTI identity, BIRIKYU registry ID,
 * descriptor/factory relationship, and object shape; later EAD lineage
 * supplies the spelling prior. Exact original SM64DS symbols are not
 * preserved. Historical project aliases: Amp_Spawn and Amp_SpawnInfo. The
 * private state-machine spellings are inferred; their class ownership,
 * bodies, calls, PMF layout, and ordering are proven.
 *
 * mwccarm emits ordinary function sections in reverse source order. Keep the
 * ROM-high InitResources method first and the destructor group last.
 * InitResources is the key function; together with the inline destructor in
 * the real header it naturally emits retail D1 then D0 and the complete class
 * data group.
 */

#include "common.h"
#include "daBrq_c.h"

/* mwccarm cannot express placement construction against retail's fBase_c
 * allocator. Keep that measured factory in its exact C source; this genuine
 * class TU owns the descriptor, members, destructor, RTTI, and vtable. */
extern "C" int *daBrq_c_classInit(void);

struct BrqSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char BrqSpawnInfo_size_must_be_0x1c[
    sizeof(BrqSpawnInfo) == 0x1c ? 1 : -1];

extern "C" BrqSpawnInfo g_profile_BIRIKYU = {
    daBrq_c_classInit,
    0x010a,
    0x007d,
    0x00000003,
    0,
    0x00028000,
    0x01000000,
    0x00a28000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN7daBrq_c13InitResourcesEv, 0x02120eec, size 0x1c0 */
/* -------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method */
#include "TextureSequence.h"
struct SharedFilePtr;
struct BMD_File;
struct BTA_File;
struct dActor_c;
struct Vector3;
struct Vector3_16;

extern "C" void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(dActor_c *self, int a, int b, int c, int d);
extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, dActor_c *a, Vector3 const &b, int c, int d, unsigned int e, unsigned int f);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *a, int b, int c, Vector3_16 *d, Vector3_16 *e);

extern SharedFilePtr data_ov070_021235fc;
extern SharedFilePtr data_ov070_02123604;
extern SharedFilePtr *data_ov070_021222e0[];
extern SharedFilePtr data_ov070_021235ec;
extern BTA_File data_ov070_021231f4;
extern Vector3 data_ov070_0212365c;
extern char IDENTITY_MATRIX4X3;

struct BrqMatrixWords { int words[12]; };

// @symbol _ZN7daBrq_c13InitResourcesEv
int daBrq_c::InitResources()
{
    BMD_File *bmd;
    bmd = (BMD_File *)Model::LoadFile(data_ov070_021235fc);
    mModelAnim.SetFile(bmd, 1, 1);
    bmd = (BMD_File *)Model::LoadFile(data_ov070_02123604);
    mModel.SetFile(bmd, 1, 1);

    int i;
    for (i = 0; i < 2; i++) {
        Animation::LoadFile(*data_ov070_021222e0[i]);
    }

    BMD_File *bmd2 = *(BMD_File **)((char *)&data_ov070_02123604 + 4);
    BTP_File *btp = (BTP_File *)TextureSequence::LoadFile(data_ov070_021235ec);
    TextureSequence::Prepare(*bmd2, *btp);

    bmd2 = *(BMD_File **)((char *)&data_ov070_02123604 + 4);
    TextureTransformer::Prepare(*bmd2, data_ov070_021231f4);

    if (!mShadowModel.InitCylinder())
        return 0;

    if ((unsigned char)((param1 >> 1) & 1)) {
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0, 0x20d000, 0x1000000, 0xa28000);
    } else {
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0, 0x2c1000, 0x1000000, 0xa28000);
    }

    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, data_ov070_0212365c, 0x2d000, 0x50000, 0x200002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x2d000, 0x2d000, 0, 0);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    SetState(1);

    *(BrqMatrixWords *)&mMat4x3 =
        *(BrqMatrixWords *)&IDENTITY_MATRIX4X3;

    UpdateModelTransform();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN7daBrq_c8BehaviorEv, 0x02120e8c, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daBrq_c8BehaviorEv
int daBrq_c::Behavior()
{
    UpdateState();
    mCylinderOffset.y += data_ov070_0212365c.y;  /* was int[] view's [1]; same word */
    mdCcAcPos_c.SetPosRelativeToActor(mCylinderOffset);
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    UpdateModelTransform();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN7daBrq_c6RenderEv, 0x02120e24, size 0x68 */
/* -------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method */
// @symbol _ZN7daBrq_c6RenderEv
int daBrq_c::Render()
{
    mModelAnim.Render(0);

    if (mState != 0 && mState != 2) {
        mTextureSequence.Update(mModel.data);
        mTextureTransformer.Update(mModel.data);
        mModel.Render((Vector3 *)&mScaleX);
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN7daBrq_c16OnPendingDestroyEv, 0x02120e20, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daBrq_c16OnPendingDestroyEv
void daBrq_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN7daBrq_c16CleanupResourcesEv, 0x02120dc4, size 0x5c */
/* -------------------------------------------------------------------------- */
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov070_021235fc;
extern SharedFilePtr data_ov070_02123604;
extern SharedFilePtr *data_ov070_021222e0[2];
extern SharedFilePtr data_ov070_021235ec;

// @symbol _ZN7daBrq_c16CleanupResourcesEv
int daBrq_c::CleanupResources()
{
    data_ov070_021235fc.Release();
    data_ov070_02123604.Release();

    int i = 0;
    do {
        data_ov070_021222e0[i]->Release();
        i++;
    } while (i < 2);

    data_ov070_021235ec.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov070_02120da8, 0x02120da8, size 0x1c */
/* -------------------------------------------------------------------------- */
extern BrqStateHandlers data_ov070_02123668[];
// @symbol _ZN7daBrq_c8SetStateEi
void daBrq_c::SetState(s32 state)
{
    mStateHandlers = &data_ov070_02123668[state];
    EnterState();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov070_02120d70, 0x02120d70, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daBrq_c10EnterStateEv
void daBrq_c::EnterState()
{
    BrqStateHandler *handler = &mStateHandlers->enter;
    (this->**handler)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov070_02120d34, 0x02120d34, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daBrq_c11UpdateStateEv
void daBrq_c::UpdateState()
{
    BrqStateHandler *handler = &mStateHandlers->update;
    (this->**handler)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov070_02120ce4, 0x02120ce4, size 0x50 */
/* -------------------------------------------------------------------------- */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *model, BCA_File *file, int flags, int speed, u32 startFrame);
extern int data_ov070_0212360c[];
// @symbol _ZN7daBrq_c18EnterCooldownStateEv
s32 daBrq_c::EnterCooldownState()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (BCA_File *)data_ov070_0212360c[1],
        0x40000000, 0x1000, 0);
    mStateTimer = 0x3c;
    mState = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov070_02120cac, 0x02120cac, size 0x38 */
/* -------------------------------------------------------------------------- */
/* (Animation: real header type in scope) */

extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);

// @symbol _ZN7daBrq_c19UpdateCooldownStateEv
s32 daBrq_c::UpdateCooldownState()
{
    mModelAnim.Advance();
    unsigned char r = DecIfAbove0_Byte(&mStateTimer);
    if (r == 0) {
        SetState(1);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov070_02120bf8, 0x02120bf8, size 0xb4 */
/* -------------------------------------------------------------------------- */
typedef int Fix12i;
struct BCA_File; struct BTP_File; struct BTA_File;
/* (ModelAnim/TextureSequence/TextureTransformer: real header types in scope) */

extern "C" unsigned int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    TextureSequence*, BTP_File&, int, Fix12i, unsigned int);
extern "C" unsigned int _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    TextureTransformer*, BTA_File&, int, Fix12i, unsigned int);

struct BrqAnimationResource { int state; BCA_File *file; };
struct BrqTextureResource { int state; BTP_File *file; };
extern BrqAnimationResource data_ov070_021235f4;
extern BTA_File data_ov070_021231f4;

// @symbol _ZN7daBrq_c16EnterActiveStateEv
s32 daBrq_c::EnterActiveState()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, data_ov070_021235f4.file, 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTextureSequence,
        *((BrqTextureResource *)&data_ov070_021235ec)->file,
        0, 0x1000, 0);
    mTextureSequence.speed = 0x1000;
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, data_ov070_021231f4, 0, 0x1000, 0);
    mTextureTransformer.speed = 0x2000;
    mStateTimer = 0xf;
    mTurnSpeed = 0;
    mSpinAngle = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mState = 1;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov070_021209e4, 0x021209e4, size 0x214 */
/* -------------------------------------------------------------------------- */
/* recovered: real class fields and member state handler */
void ApproachLinear(s32 &value, s32 target, s32 step);
extern "C" void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
namespace Sound {
u32 PlayLong(u32 handle, u32 bank, u32 soundId,
             const Vector3 &pos, s16 pitch);
}
extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);
extern short data_02082214[];

// @symbol _ZN7daBrq_c17UpdateActiveStateEv
s32 daBrq_c::UpdateActiveState()
{
    ApproachLinear(mTurnSpeed, 1000, 20);

    unsigned char bit0 = (unsigned char)(param1 & 1);
    if (bit0) {
        mPrevAngleY = (short)(mPrevAngleY + mTurnSpeed);
    } else {
        mPrevAngleY = (short)(mPrevAngleY - mTurnSpeed);
    }

    mOrbitAngle = (short)(mOrbitAngle + 0xa00);

    unsigned char bit1 = (unsigned char)((param1 >> 1) & 1);
    int ip = bit1 ? 0xb4000 : 0x168000;

    mOrbitCenter.x = mPosX;
    mOrbitCenter.y = mPosY;
    mOrbitCenter.z = mPosZ;

    {
        int idx = ((unsigned short)mPrevAngleY) >> 4;
        int cosv = data_02082214[idx * 2];
        mCylinderOffset.x = (int)(((s64)ip * cosv + 0x800) >> 0xc);
    }
    {
        int idx = ((unsigned short)mOrbitAngle) >> 4;
        int sinv = data_02082214[idx * 2];
        mCylinderOffset.y = (int)(((s64)0x14000 * sinv + 0x800) >> 0xc);
    }
    {
        int idx = ((unsigned short)mPrevAngleY) >> 4;
        int sinv = data_02082214[idx * 2 + 1];
        mCylinderOffset.z = (int)(((s64)ip * sinv + 0x800) >> 0xc);
    }

    AddVec3(&mOrbitCenter, &mCylinderOffset, &mOrbitCenter);

    mSpinAngle = (short)(mSpinAngle + 0x4000);

    {
        int idx = ((unsigned short)mSpinAngle) >> 4;
        int cosv = data_02082214[idx * 2];
        int scale = (int)(((s64)0x555 * cosv + 0x800) >> 0xc) + 0x800;
        mScaleX = scale;
        mScaleY = scale;
        mScaleZ = scale;
    }

    mModelAnim.Advance();
    mTextureSequence.Advance();
    mTextureTransformer.Advance();

    mSoundHandle = Sound::PlayLong(
        mSoundHandle, 3, 0x183, *(Vector3 *)&mCamSpacePosX, 0);

    if (DecIfAbove0_Byte(&mStateTimer) == 0) {
        HandlePlayerCollision();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov070_02120910, 0x02120910, size 0xd4 */
/* -------------------------------------------------------------------------- */
/* recovered: real class fields and member state handler */
namespace Sound {
void PlayBank0(u32 soundId, const Vector3 &pos);
}
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 effectId, s32 x, s32 y, s32 z);

// @symbol _ZN7daBrq_c18EnterDefeatedStateEv
s32 daBrq_c::EnterDefeatedState()
{
    Sound::PlayBank0(9, *(Vector3 *)&mCamSpacePosX);
    mFlags &= ~1;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mPosX = mOrbitCenter.x;
    mPosY = mOrbitCenter.y;
    mPosZ = mOrbitCenter.z;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mStateTimer = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (BCA_File *)data_ov070_0212360c[1],
        0x40000000, 0x1000, 0);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x43, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x44, mPosX, mPosY, mPosZ);
    mState = 2;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov070_021208a4, 0x021208a4, size 0x6c */
/* -------------------------------------------------------------------------- */
/* The ROM calls the interworking veneer at 0x02038420, not the direct
 * dBgCh_Actr::UpdateDiscreteNoLava body at 0x02037024. */
extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *collision);

// @symbol _ZN7daBrq_c19UpdateDefeatedStateEv
s32 daBrq_c::UpdateDefeatedState()
{
    mAngleX = (s16)(mAngleX - 0x1000);
    mModelAnim.Advance();
    UpdatePos(&mdCcAcPos_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);

    if (mWithMeshClsn.JustHitGround() == 0) {
        if (DecIfAbove0_Byte(&mStateTimer) != 0)
            goto done;
    }

    PoofDust();
    MarkForDestruction();
done:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov070_02120724, 0x02120724, size 0x180 */
/* -------------------------------------------------------------------------- */
#include "dBgCh_Gnd.h"
extern "C" void Matrix4x3_FromRotationXYZExt(
    Matrix4x3 *matrix, s16 x, s16 y, s16 z);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    s32 radius, s32 depth, u32 opacity);
extern signed char data_0209f2f8[];

struct BrqVector3Words { int x, y, z; };

// @symbol _ZN7daBrq_c20UpdateModelTransformEv
void daBrq_c::UpdateModelTransform()
{
    int g;

    if (mState == 2) {
        Matrix4x3_FromRotationXYZExt(
            &mModelAnim.mat4x3, mAngleX, mAngleY, mAngleZ);
        mModelAnim.mat4x3.m[9] = mPosX >> 3;
        mModelAnim.mat4x3.m[10] = mPosY >> 3;
        mModelAnim.mat4x3.m[11] = mPosZ >> 3;
        mMat4x3.m[9] = mPosX >> 3;
        mMat4x3.m[10] = mPosY >> 3;
        mMat4x3.m[11] = mPosZ >> 3;
    } else {
        mModelAnim.mat4x3.m[9] = mOrbitCenter.x >> 3;
        mModelAnim.mat4x3.m[10] = mOrbitCenter.y >> 3;
        mModelAnim.mat4x3.m[11] = mOrbitCenter.z >> 3;
        mModel.mat4x3 = mModelAnim.mat4x3;
        mMat4x3.m[9] = mOrbitCenter.x >> 3;
        mMat4x3.m[10] = mOrbitCenter.y >> 3;
        mMat4x3.m[11] = mOrbitCenter.z >> 3;
    }

    g = 0x1f4000;
    if (data_0209f2f8[0] == 0x11) {
        BrqVector3Words pos;
        int y;
        pos.x = mPosX;
        y = mPosY;
        pos.y = y;
        pos.z = mPosZ;
        pos.y = y - 0xa000;
        dBgCh_Gnd ground;
        ground.SetObjAndPos(*(Vector3 *)&pos, 0);
        if (ground.DetectClsn() != 0) {
            g = (mPosY - ground.clsnY) + 0x28000;
        }
    }

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mMat4x3, 0x5a000, g, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov070_02120644, 0x02120644, size 0xe0 */
/* -------------------------------------------------------------------------- */
#include "Player.h"
extern "C" short Vec3_HorzAngle(const Vector3 *from, const Vector3 *to);

// @symbol _ZN7daBrq_c21HandlePlayerCollisionEv
s32 daBrq_c::HandlePlayerCollision()
{
    u32 id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        return 0;

    dActor_c *other = dActor_c::FindWithID(id);
    if (other == 0)
        goto notPlayer;

    {
        int isPlayer = (other->actorID == 0xbf);
        if (isPlayer != 0)
            goto foundPlayer;
    }

notPlayer:
    return 0;

foundPlayer:
    Player *player = (Player *)other;
    if (player->mIsVanish != 0)
        return 0;

    if (mdCcAcPos_c.hitFlags & 0x10) {
        mPrevAngleY = Vec3_HorzAngle(
            (Vector3 *)&player->mPosX, (Vector3 *)&mPosX);
        mAngleY = (short)(mPrevAngleY + 0x8000);
        player->IncMegaKillCount();
        SetState(2);
    } else {
        if (player->Shock(1) != 0)
            SetState(0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN7daBrq_cD0Ev, 0x021205d0, size 0x74 */
/* -------------------------------------------------------------------------- */
/* The inline destructor and InitResources key function emit the retail D1/D0
 * group without a retained D2 or forcing helper. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN7daBrq_cD1Ev, 0x02120570, size 0x60 */
/* -------------------------------------------------------------------------- */
