//cpp
/* Production translation unit for ov065/daObjCtMecha04_c.
 * Nine functions, .text 0x0211a494..0x0211aae0.
 *
 * The class identity and direct dBgActor_c base are ROM-proven by RTTI. The
 * helper names UpdateShadow, MoveActorOnBelt, and AfterClsnCallback remain
 * descriptive reconstructions. The latter two symbols also encode inferred
 * member/static and parameter spellings; see the class declaration and TU
 * manifest for the exact evidence boundary.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
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

/* Includes reconciled from the legacy shards; this order is part of the
 * verified 2004/b56 TU context. */
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

extern "C" {
extern void Matrix4x3_FromRotationY(Matrix4x3 *matrix, s16 angle);
extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    int scaleX, int scaleY, int scaleZ, u32 opacity);
extern char data_ov065_0211d194[];
extern char data_ov065_0211d198[];
extern char data_ov065_0211d19c[];
extern void func_020393c4(void *p, void *v);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
extern int _Z14ApproachLinearRiii(int* r, int t, int step);
extern u16 DecIfAbove0_Short(u16* p);
extern int RandomIntInternal(int* seed);
extern void* _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int d);
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
  u16 ang = (u16)mAngleY;
  int idx = ang >> 4;
  int v = mBeltSpeed << 2;
  int c = data_02082214[idx * 2];
  int res1 = (int)((((s64)v * c) + 0x800) >> 12);
  s32 *actorPosX = &actor.mPosX;
  int oldX = *actorPosX;
  *actorPosX = oldX + res1;

  u16 ang2 = (u16)mAngleY;
  int idx2 = ang2 >> 4;
  int v2 = mBeltSpeed << 2;
  s32 *actorPosZ = actorPosX + 2;
  int s = data_02082214[(idx2 * 2) + 1];
  int oldZ = *actorPosZ;
  int res2 = (int)((((s64)v2 * s) + 0x800) >> 12);
  *actorPosZ = oldZ + res2;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN16daObjCtMecha04_c13InitResourcesEv, 0x0211a870, size 0x1c8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha04_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* ModelBase is the real class now, through daObjCtMecha04_c.h. */
/* ShadowModel is the real class now, through this actor's header, and it
   declares InitCuboid itself. */
/* TextureTransformer is the real class now, through this actor's header, which
   types mTextureTransformer. Redefining it ICEs mwccarm (CClass.c:3328); SetFile
   is still reached by its mangled symbol below, because the real one takes
   Fix12<int> BY VALUE and mwccarm passes that differently at the call site. */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
/* dBgW_KcMbg is the real class now, through this actor's header. Only the
   cast below needs the name; SetFile is already reached by its mangled
   symbol because the real one takes Fix12<int> by value. */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
int daObjCtMecha04_c::InitResources()
{
    void *locbuf[2];
    Vector3 v;
    unsigned char e;
    void *mf;
    void *kf;

    locbuf[0] = *(void **)&data_ov065_0211d16c[0];
    locbuf[1] = *(void **)&data_ov065_0211d16c[4];

    if (actorID != 0x6f) {
        if (actorID == 0x70)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    e = mVariant;
    mf = Model::LoadFile(
        *(SharedFilePtr *)((Entry3 *)data_ov065_0211d194)[e].a);
    mModel.SetFile(
        (BMD_File *)mf, 1, -1);

    mShadowModel.InitCuboid();

    e = mVariant;
    TextureTransformer::Prepare(
        *(BMD_File *)*(void **)((char *)((Entry3 *)data_ov065_0211d194)[e].a + 4),
        *(BTA_File *)locbuf[e]);

    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj((TextureTransformer *)((char *)&mTextureTransformer), *(BTA_File *)locbuf[mVariant], 0, 0x1000, 0);

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    e = mVariant;
    kf = dBgW_Kc::LoadFile(
        *(SharedFilePtr *)((Entry3 *)data_ov065_0211d198)[e].a);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, (KCL_File *)kf, mClsnMat, 0x199, mAngleY, *(CLPS_Block *)((Entry3 *)data_ov065_0211d19c)[e].a);

    func_020393c4(
        &mMeshCollider,
        (void *)&daObjCtMecha04_c::AfterClsnCallback);

    mTargetBeltSpeed = data_ov065_0211c0b8[data_0209f2c0];
    mBeltSpeed = mTargetBeltSpeed;
    /* 0x32c is +0xc inside the TextureTransformer at 0x320 -- its Animation base's
       speed. The cartridge's own ~daObjCtMecha04_c proves the extent; see
       tools/dtor_members.py. */
    mTextureTransformer.speed = mBeltSpeed;

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = v.y - 0xa000;

    {
        dBgCh_Gnd rg;

        rg.SetObjAndPos(v, (dActor_c *)0);
        mGroundY = v.y;

        if (rg.DetectClsn() != 0)
            mGroundY = rg.clsnY;
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN16daObjCtMecha04_c8BehaviorEv, 0x0211a6d0, size 0x1a0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha04_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* _ZN16daObjCtMecha04_c8BehaviorEv at 0x0211a6d0 (ov065), size 0x1a0
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
int daObjCtMecha04_c::Behavior()
{
    if (data_0209f2c0 == 3) {
        func_020393c4((int*)((char*)&mMeshCollider), 0);
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0);
    } else {
        if (((mFlags & 8) ? 1 : 0) == 0) {
            if (func_020393bc((int*)((char*)&mMeshCollider)) == 0) {
                func_020393c4((int*)((char*)&mMeshCollider),
                              (void *)&daObjCtMecha04_c::AfterClsnCallback);
            }

            if (data_0209f2c0 == 2) {
                if (_Z14ApproachLinearRiii((int*)((char*)&mBeltSpeed), mTargetBeltSpeed, 0xcc) != 0
                    && DecIfAbove0_Short((u16*)((char*)&mDirectionTimer)) == 0) {
                    unsigned int r = (u16)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
                    mDirectionTimer = (s16)(((int)r % 7) * 0x14 + 0xa);
                    if (r >= 0x7fff) {
                        mTargetBeltSpeed = 0x1000;
                    } else {
                        mTargetBeltSpeed = -0x1000;
                    }
                }
            } else {
                mBeltSpeed = data_ov065_0211c0b8[data_0209f2c0];
            }

            /* +0xc inside the TextureTransformer at 0x320: its Animation base's speed. */
            mTextureTransformer.speed = mBeltSpeed;
            mTextureTransformer.Advance();
            if (mBeltSpeed != 0) {
                mSoundHandle = (int)_ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 3, 0x88, ((char*)this) + 0x74, 0);
            }
        }

        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0);
    }

    UpdateShadow();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN16daObjCtMecha04_c6RenderEv, 0x0211a69c, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha04_c6RenderEv
/* recovered: named members + shared header, real C++ method */
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
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
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
