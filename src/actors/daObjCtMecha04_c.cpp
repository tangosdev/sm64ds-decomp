//cpp
/* Production translation unit for ov065/daObjCtMecha04_c.
 *
 * mwccarm emits ordinary functions in reverse source order, so the nine
 * definitions below intentionally run from the highest retail address back
 * toward the compiler-owned destructor group.
 *
 * UpdateShadow, MoveActorOnBelt, and AfterClsnCallback are descriptive
 * reconstructions. The member/static forms and parameter spellings of the
 * latter two are also inferred; the manifest records the evidence boundary.
 *
 * Superseded one-function sources (ROM address order):
 *   [0] 0x0211a494  src/_ZN16daObjCtMecha04_cD1Ev.cpp
 *   [1] 0x0211a4e8  src/_ZN16daObjCtMecha04_cD0Ev.cpp
 *   [2] 0x0211a550  src/_ZN16daObjCtMecha04_c12UpdateShadowEv.cpp
 *   [3] 0x0211a638  src/_ZN16daObjCtMecha04_c16CleanupResourcesEv.cpp
 *   [4] 0x0211a69c  src/_ZN16daObjCtMecha04_c6RenderEv.cpp
 *   [5] 0x0211a6d0  src/_ZN16daObjCtMecha04_c8BehaviorEv.cpp
 *   [6] 0x0211a870  src/_ZN16daObjCtMecha04_c13InitResourcesEv.cpp
 *   [7] 0x0211aa38  src/_ZN16daObjCtMecha04_c15MoveActorOnBeltER8dActor_c.cpp
 *   [8] 0x0211aacc  src/_ZN16daObjCtMecha04_c17AfterClsnCallbackEP4dBgWP8dActor_cS3_.cpp
 */

#include "daObjCtMecha04_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "types.h"
#include "dBgCh_Gnd.h"

/* The three resource tables are arrays of 12-byte records. Their entries
 * retain address-based names because the ROM does not preserve source names. */
struct Entry3 {
    void *a;
    void *b;
    void *c;
};

/* Fix12-by-value calls retain their measured raw ABI declarations. Natural
 * class-typed declarations make mwccarm home arguments absent from retail. */
extern "C" {
extern void Matrix4x3_FromRotationY(Matrix4x3 *matrix, s16 angle);
extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    int scaleX, int scaleY, int scaleZ, u32 opacity);
extern char data_ov065_0211d194[];
extern char data_ov065_0211d198[];
extern char data_ov065_0211d19c[];
extern void func_020393c4(void *p, void *v);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern int _Z14ApproachLinearRiii(int *r, int t, int step);
extern u16 DecIfAbove0_Short(u16 *p);
extern int RandomIntInternal(int *seed);
extern void *_ZN5Sound8PlayLongEjjjRK7Vector3s(
    unsigned int a, unsigned int b, unsigned int cc,
    void *v, unsigned int d);
extern u8 data_0209f2c0;
extern int data_0209e650;
extern int data_ov065_0211c0b8[];
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    void *, BTA_File &f, int a, int fix, unsigned int u);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *, KCL_File *f, const Matrix4x3 &m, int fix, short sh,
    CLPS_Block &b);
extern s16 data_02082214[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN16daObjCtMecha04_c17AfterClsnCallbackEP4dBgWP8dActor_cS3_, 0x0211aacc, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha04_c17AfterClsnCallbackEP4dBgWP8dActor_cS3_
/* Inferred descriptive name and observed three-register callback ABI. The
 * wrapper deliberately ignores the collider and tail-calls the owner method. */
void daObjCtMecha04_c::AfterClsnCallback(dBgW *collider, dActor_c *owner,
                                         dActor_c *other)
{
    ((daObjCtMecha04_c *)owner)->MoveActorOnBelt(*other);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN16daObjCtMecha04_c15MoveActorOnBeltER8dActor_c, 0x0211aa38, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha04_c15MoveActorOnBeltER8dActor_c
/* Inferred descriptive name. The collision callback supplies this conveyor as
 * owner and the actor whose X/Z position should advance with the belt. */
void daObjCtMecha04_c::MoveActorOnBelt(dActor_c &actor)
{
    u16 angleForX = (u16)mAngleY;
    int angleIndexForX = angleForX >> 4;
    int beltStepForX = mBeltSpeed << 2;
    int cosine = data_02082214[angleIndexForX * 2];
    int deltaX = (int)((((s64)beltStepForX * cosine) + 0x800) >> 12);
    s32 *actorPosX = &actor.mPosX;
    int oldX = *actorPosX;
    *actorPosX = oldX + deltaX;

    u16 angleForZ = (u16)mAngleY;
    int angleIndexForZ = angleForZ >> 4;
    int beltStepForZ = mBeltSpeed << 2;
    s32 *actorPosZ = actorPosX + 2;
    int sine = data_02082214[(angleIndexForZ * 2) + 1];
    int oldZ = *actorPosZ;
    int deltaZ = (int)((((s64)beltStepForZ * sine) + 0x800) >> 12);
    *actorPosZ = oldZ + deltaZ;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN16daObjCtMecha04_c13InitResourcesEv, 0x0211a870, size 0x1c8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha04_c13InitResourcesEv
int daObjCtMecha04_c::InitResources()
{
    void *animationFiles[2];
    Vector3 position;
    unsigned char variant;
    void *modelFile;
    void *collisionFile;

    animationFiles[0] = *(void **)&data_ov065_0211d16c[0];
    animationFiles[1] = *(void **)&data_ov065_0211d16c[4];

    if (actorID != 0x6f) {
        if (actorID == 0x70)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    variant = mVariant;
    modelFile = Model::LoadFile(
        *(SharedFilePtr *)((Entry3 *)data_ov065_0211d194)[variant].a);
    mModel.SetFile(
        (BMD_File *)modelFile, 1, -1);

    mShadowModel.InitCuboid();

    variant = mVariant;
    TextureTransformer::Prepare(
        *(BMD_File *)*(void **)((char *)((Entry3 *)data_ov065_0211d194)[variant].a + 4),
        *(BTA_File *)animationFiles[variant]);

    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        (TextureTransformer *)((char *)&mTextureTransformer),
        *(BTA_File *)animationFiles[mVariant], 0, 0x1000, 0);

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    variant = mVariant;
    collisionFile = dBgW_Kc::LoadFile(
        *(SharedFilePtr *)((Entry3 *)data_ov065_0211d198)[variant].a);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)collisionFile, mClsnMat,
        0x199, mAngleY,
        *(CLPS_Block *)((Entry3 *)data_ov065_0211d19c)[variant].a);

    func_020393c4(
        &mMeshCollider,
        (void *)&daObjCtMecha04_c::AfterClsnCallback);

    mTargetBeltSpeed = data_ov065_0211c0b8[data_0209f2c0];
    mBeltSpeed = mTargetBeltSpeed;
    mTextureTransformer.speed = mBeltSpeed;

    position.x = mPosX;
    position.y = mPosY;
    position.z = mPosZ;
    position.y = position.y - 0xa000;

    {
        dBgCh_Gnd ground;

        ground.SetObjAndPos(position, (dActor_c *)0);
        mGroundY = position.y;

        if (ground.DetectClsn() != 0)
            mGroundY = ground.clsnY;
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN16daObjCtMecha04_c8BehaviorEv, 0x0211a6d0, size 0x1a0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha04_c8BehaviorEv
int daObjCtMecha04_c::Behavior()
{
    if (data_0209f2c0 == 3) {
        func_020393c4((int *)((char *)&mMeshCollider), 0);
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0);
    } else {
        if (((mFlags & 8) ? 1 : 0) == 0) {
            if (func_020393bc((int *)((char *)&mMeshCollider)) == 0) {
                func_020393c4((int *)((char *)&mMeshCollider),
                              (void *)&daObjCtMecha04_c::AfterClsnCallback);
            }

            if (data_0209f2c0 == 2) {
                if (_Z14ApproachLinearRiii(
                        (int *)((char *)&mBeltSpeed), mTargetBeltSpeed, 0xcc) != 0
                    && DecIfAbove0_Short((u16 *)((char *)&mDirectionTimer)) == 0) {
                    unsigned int randomValue = (u16)(
                        (unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
                    mDirectionTimer = (s16)(((int)randomValue % 7) * 0x14 + 0xa);
                    if (randomValue >= 0x7fff) {
                        mTargetBeltSpeed = 0x1000;
                    } else {
                        mTargetBeltSpeed = -0x1000;
                    }
                }
            } else {
                mBeltSpeed = data_ov065_0211c0b8[data_0209f2c0];
            }

            mTextureTransformer.speed = mBeltSpeed;
            mTextureTransformer.Advance();
            if (mBeltSpeed != 0) {
                mSoundHandle = (int)_ZN5Sound8PlayLongEjjjRK7Vector3s(
                    mSoundHandle, 3, 0x88, ((char *)this) + 0x74, 0);
            }
        }

        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0);
    }

    UpdateShadow();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN16daObjCtMecha04_c6RenderEv, 0x0211a69c, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha04_c6RenderEv
int daObjCtMecha04_c::Render()
{
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN16daObjCtMecha04_c16CleanupResourcesEv, 0x0211a638, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha04_c16CleanupResourcesEv
int daObjCtMecha04_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)(*(void **)(data_ov065_0211d194 + mVariant * 0xc)))->Release();
    ((SharedFilePtr *)(*(void **)(data_ov065_0211d198 + mVariant * 0xc)))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN16daObjCtMecha04_c12UpdateShadowEv, 0x0211a550, size 0xe8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha04_c12UpdateShadowEv
/* Inferred descriptive name. The owned ShadowModel and its matrix are fixed by
 * the destructor, field accesses, and dActor_c::DropShadowScaleXYZ call. */
/* The real declaration takes three Fix12<int> values by value. This compiler
 * homes those class-typed arguments in the caller, unlike the cartridge call;
 * keep the measured register/stack ABI at this one boundary (notes 6az). */
void daObjCtMecha04_c::UpdateShadow()
{
    int heightDiff = mPosY - mGroundY;
    int absHeightDiff = heightDiff < 0 ? -heightDiff : heightDiff;
    if (absHeightDiff > 0x7d0000)
        return;

    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = (mGroundY + 0x1000) >> 3;
    mShadowMat.m[11] = mPosZ >> 3;

    int isLarge = (int)(actorID == 0x6f);
    if (isLarge != 0) {
        _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            this, &mShadowModel, &mShadowMat,
            0x1f4000, 0x32000, 0x3e8000, 0xf);
        return;
    }

    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        this, &mShadowModel, &mShadowMat,
        0x1f4000, 0x32000, 0x320000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN16daObjCtMecha04_cD0Ev, 0x0211a4e8, size 0x68        */
/* ROM ordinal 0 -- _ZN16daObjCtMecha04_cD1Ev, 0x0211a494, size 0x54        */
/* -------------------------------------------------------------------------- */
/* No separate body lives here. The inline virtual destructor in the directly
 * included class header makes mwccarm emit retail's D1 then D0 order without
 * the otherwise homeless D2 variant. */
