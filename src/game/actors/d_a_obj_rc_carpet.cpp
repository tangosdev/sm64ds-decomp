//cpp
#include "daObjRcCarpet_c.h"
#include "SharedFilePtr.h"

/* Manually curated translation unit -- ov036/daObjRcCarpet_c (9 functions).
 * The ordinary definitions remain in reverse ROM order because mwccarm emits
 * one text section per function in reverse source order. The inline class
 * destructor is the exception: vtable instantiation makes the compiler emit
 * the retail D1 then D0 pair first, with no D2.
 */

struct FlyingCarpetResources {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

typedef char FlyingCarpetResources_size_must_be_0x0c[
    sizeof(FlyingCarpetResources) == 0x0c ? 1 : -1];

struct FlyingCarpetSpawnInfo {
    daObjRcCarpet_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char FlyingCarpetSpawnInfo_size_must_be_0x1c[
    sizeof(FlyingCarpetSpawnInfo) == 0x1c ? 1 : -1];

struct M4x3Flat {
    s32 m[12];
};

extern "C" {
extern SharedFilePtr data_ov002_0210d9f0;
extern SharedFilePtr data_ov036_0211419c;
extern SharedFilePtr data_ov036_021141a4;
extern SharedFilePtr data_ov036_021141ac;
extern CLPS_Block data_ov036_02112b28;

/* Retail resource-pointer table at ov036:0x02113f58. */
FlyingCarpetResources data_ov036_02113f58 = {
    &data_ov036_021141a4,
    &data_ov036_021141ac,
    &data_ov036_02112b28
};

extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN10dBgActor_cC2Ev(dBgActor_c *actor);
extern void _ZN5ModelC1Ev(Model *model);
extern void _ZN5ModelD1Ev(Model *model);
extern void _ZN7PathPtrC1Ev(PathPtr *path);
extern void _ZN9ModelAnimC1Ev(ModelAnim *model);
extern void func_020733a8(
    Model *models, int count, int size, void *ctor, void *dtor);
extern int _ZTV15daObjRcCarpet_c[];
extern int data_ov002_0210af70[];

extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *model, BCA_File *file, int flags, Fix12i speed,
    unsigned int startFrame);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *collider, KCL_File *file, const Matrix4x3 *matrix,
    Fix12i scale, s16 angle, CLPS_Block *clps);
extern void func_020393d4(dBgW_KcMbg *collider, void *callback);
extern void func_ov002_020efaf0(dPathLiftActor_c *lift);
extern void _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
extern void _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
    dBgActor_c *actor, Fix12i low, Fix12i high);
extern void Matrix4x3_FromRotationXYZExt(
    Matrix4x3 *matrix, s16 x, s16 y, s16 z);
}

int ApproachLinear(int &value, int target, int step);

extern "C" void func_ov036_021122c0(daObjRcCarpet_c *self);
extern "C" void func_ov036_0211224c(daObjRcCarpet_c *self);

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- daObjRcCarpet_c_classInit, 0x02112538, size 0x78 */
/* -------------------------------------------------------------------------- */
/* Natural new targets the global allocator and cannot reproduce the actor
 * allocation/construction relocations. Keep this typed C-ABI factory seam. */
/* Reconstructed source-style name. Historical alias: FlyingCarpet_Spawn. */
extern "C" daObjRcCarpet_c *daObjRcCarpet_c_classInit()
{
    daObjRcCarpet_c *actor =
        (daObjRcCarpet_c *)_ZN7fBase_cnwEj(sizeof(daObjRcCarpet_c));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *(int *)actor = (int)data_ov002_0210af70;
        func_020733a8(
            actor->mModels, 3, sizeof(Model),
            (void *)_ZN5ModelC1Ev, (void *)_ZN5ModelD1Ev);
        _ZN7PathPtrC1Ev(&actor->mPath);
        *(int *)actor = (int)&_ZTV15daObjRcCarpet_c[2];
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
    }
    return actor;
}

extern "C" FlyingCarpetSpawnInfo g_profile_RC_CARPET = {
    daObjRcCarpet_c_classInit,
    0x0082,
    0x0104,
    0x00000002,
    0x00000000,
    0x00300000,
    0x01000000,
    0x00000000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- InitResources, 0x0211244c, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjRcCarpet_c13InitResourcesEv
int daObjRcCarpet_c::InitResources()
{
    Model::LoadFile(data_ov002_0210d9f0);
    void *modelFile = Model::LoadFile(*data_ov036_02113f58.model);
    mModelAnim.SetFile((BMD_File *)modelFile, 1, -1);
    Animation::LoadFile(data_ov036_0211419c);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (BCA_File *)((int *)&data_ov036_0211419c)[1],
        0, 0x1000, 0);
    func_ov036_021122c0(this);
    UpdateClsnPosAndRot();
    KCL_File *collisionFile =
        (KCL_File *)dBgW_Kc::LoadFile(*data_ov036_02113f58.collision);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, collisionFile, &mClsnMat,
        0x199, mAngleY, data_ov036_02113f58.clps);
    func_020393d4(
        &mMeshCollider,
        (void *)_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    mPathSpeed = 0xa000;
    mHorzSpeed = mPathSpeed;
    func_ov002_020efaf0(this);
    mPathDirection = 1;
    unk_42c = 1;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- Behavior, 0x021123c8, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjRcCarpet_c8BehaviorEv
int daObjRcCarpet_c::Behavior()
{
    if (mAfterClsnRan)
        ApproachLinear(unk_4bc, -0x14000, 0x5000);
    else
        ApproachLinear(unk_4bc, 0, 0x5000);
    BaseBehavior();
    mModelAnim.Advance();
    func_ov036_021122c0(this);
    func_ov036_0211224c(this);
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- Render, 0x02112378, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjRcCarpet_c6RenderEv
int daObjRcCarpet_c::Render()
{
    if (mWaitTimer < 0x5a && (mWaitTimer & 1))
        return 1;
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- CleanupResources, 0x02112318, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjRcCarpet_c16CleanupResourcesEv
int daObjRcCarpet_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov002_0210d9f0.Release();
    data_ov036_02113f58.model->Release();
    data_ov036_02113f58.collision->Release();
    data_ov036_0211419c.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov036_021122c0, 0x021122c0, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov036_021122c0
extern "C" void func_ov036_021122c0(daObjRcCarpet_c *self)
{
    Matrix4x3_FromRotationXYZExt(
        &self->mModelAnim.mat4x3,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    /* Direct nested-member stores make b56 materialise a second 0x46c base
       and grow this function. The retail form keeps `this` as the base for
       the three translation stores, so preserve that measured expression. */
    *(s32 *)((char *)self + 0x490) = self->mPosX >> 3;
    *(s32 *)((char *)self + 0x494) =
        (self->mPosY + self->unk_4bc) >> 3;
    *(s32 *)((char *)self + 0x498) = self->mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov036_0211224c, 0x0211224c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov036_0211224c
extern "C" void func_ov036_0211224c(daObjRcCarpet_c *self)
{
    *(M4x3Flat *)&self->mClsnMat =
        *(M4x3Flat *)&self->mModelAnim.mat4x3;
    /* As above, retail keeps `this` live as the base for these stores. */
    *(s32 *)((char *)self + 0x310) = self->mPosX;
    *(s32 *)((char *)self + 0x314) = self->mPosY + self->unk_4bc;
    *(s32 *)((char *)self + 0x318) = self->mPosZ;
    self->mMeshCollider.Transform(self->mClsnMat, self->mAngleY);
}

/* ROM ordinals 0/1 are emitted by the inline virtual destructor:
 *   _ZN15daObjRcCarpet_cD1Ev  0x02112158  size 0x70
 *   _ZN15daObjRcCarpet_cD0Ev  0x021121c8  size 0x84
 */
