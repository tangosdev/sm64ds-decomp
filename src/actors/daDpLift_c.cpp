//cpp
/* Production translation unit for ov025/daDpLift_c.
 * 6 function(s), .text 0x021120e4..0x021125bc.
 *
 * The DP_LIFT pyramid lift: a dBgActor_c that waits until something stands on
 * it, shakes for eight frames, then sinks at a fixed speed down a column of
 * ten markers spaced 0x1cc000 apart, and settles with a second short shake
 * at Y 0x80000. Render draws its own model plus one mModel2 copy per marker
 * the platform has not yet passed.
 *
 * NAME: daDpLift_c is the cartridge's RTTI spelling -- _ZTS at ov025
 * 0x021139a0 is the byte string "10daDpLift_c", and _ZTI at 0x02113994 reads
 * [__si_class_type_info, that string, _ZTI10dBgActor_c].
 *
 * THE DESTRUCTOR IS THE KEY FUNCTION, declared first in the class header and
 * defined first below, so this TU emits _ZTV10daDpLift_c and the RTTI chain
 * as vague linkage. `#pragma defer_codegen off` is load-bearing twice over:
 *   - it makes mwccarm emit each function as it is parsed, so the file is
 *     written in ROM order and the out-of-line destructor comes out D1, D0
 *     (0x021120e4, 0x02112148), then a D2 the cartridge has no home for;
 *   - it makes the opt_strength_reduction bracket around InitResources
 *     bind. Without it the pragma is file-global last-wins, and the two
 *     members want opposite settings: InitResources' marker loop needs it
 *     off, and Render's indexed mBulletPositions[i] loop comes out 4 bytes
 *     long with it off (only a raw `this + i * 0xc` walk survives that).
 * MEASURED against the alternatives: with the inline-empty destructor the
 * sibling promotions use, defer_codegen off lays D1/D0 down after
 * InitResources, and without defer_codegen off the bracket does not bind.
 *
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled --
 *   both take Fix12<int> by value (wall 6az); a member call homes the
 *   argument and changes the ROM ABI.
 * Leftover: func_020393d4 / func_020393c4 are 4-byte stores into dBgW's
 *   callback slots; naming belongs with dBgW in arm9.
 * Leftover: func_ov025_021125dc, the collision callback InitResources
 *   installs, and the factory daDpLift_c_classInit (0x021125f0) sit just past
 *   this run's right edge and stay one-function sources.
 * Leftover: data_02082214 is arm9's sine/cosine table, still unnamed there.
 */

#include "daDpLift_c.h"
#include "SharedFilePtr.h"

#pragma defer_codegen off

extern "C" {
/* The lift's model file and collision file (ov025 .bss) and the marker
   model file (ov002 .bss). */
extern SharedFilePtr data_ov025_02113ae0;
extern SharedFilePtr data_ov002_0210d9f0;
extern SharedFilePtr data_ov025_02113ad8;
extern CLPS_Block data_ov025_02112d08;

extern s16 data_02082214[];

void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);

void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const Matrix4x3 *mtx, int scale, s16 angleY, void *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);

void func_020393d4(int *p, int v);
void func_020393c4(int *p, int v);
void func_ov025_021125dc(char *self, char *a, char *b);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daDpLift_cD1Ev
// @symbol _ZN10daDpLift_cD0Ev
daDpLift_c::~daDpLift_c()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daDpLift_c16CleanupResourcesEv
s32 daDpLift_c::CleanupResources()
{
    data_ov002_0210d9f0.Release();
    data_ov025_02113ae0.Release();
    data_ov025_02113ad8.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daDpLift_c6RenderEv
s32 daDpLift_c::Render()
{
    mModel.Render(0);
    for (int i = mNextBullet; i < 10; i++) {
        Matrix4x3_FromTranslation(&mModel2.mat4x3,
                                  mBulletPositions[i].x >> 3,
                                  mBulletPositions[i].y >> 3,
                                  mBulletPositions[i].z >> 3);
        mModel2.Render(0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daDpLift_c8BehaviorEv
s32 daDpLift_c::Behavior()
{
    switch (mState) {
    case 0:
        if (mHadClsn != 0) {
            mState = 1;
            mShakeTimer = 0;
        }
        break;
    case 1: {
        s16 ang = mShakeTimer << 12;
        mPosY = mBasePosY + (s32)(((s64)data_02082214[((u16)ang >> 4) * 2] * 10 + 0x800) >> 12);
        if (mShakeTimer == 8) {
            mState = 2;
            mVertSpeed = -0xa000;
        }
        mShakeTimer++;
        break;
    }
    case 2: {
        if (mPosY <= mBulletPositions[mNextBullet].y + 0x14000)
            mNextBullet++;
        mPosY += mVertSpeed;
        if (mPosY < 0x80000) {
            mPosY = 0x80000;
            mState = 3;
            mShakeTimer = 0;
        }
        break;
    }
    case 3: {
        s16 ang = mShakeTimer << 12;
        mPosY = (s32)(((s64)data_02082214[((u16)ang >> 4) * 2] * 10 + 0x800) >> 12) + 0x80000;
        if (mShakeTimer >= 8) {
            mVertSpeed = 0;
            mPosY = 0x80000;
        }
        mShakeTimer++;
        break;
    }
    }
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    mHadClsn = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN10daDpLift_c13InitResourcesEv
#pragma push
#pragma opt_strength_reduction off
s32 daDpLift_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov025_02113ae0), 1, -1);
    mModel2.SetFile((BMD_File *)Model::LoadFile(data_ov002_0210d9f0), 1, -1);
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov025_02113ad8);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, &data_ov025_02112d08);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    func_020393c4((int *)&mMeshCollider, (int)&func_ov025_021125dc);

    /* Seed the ten markers under the lift, each 0x1cc000 lower than the
       last. MEASURED: this is the spelling that reproduces. The ROM walks a
       copy of `this` by 0xc and multiplies afresh each trip -- the shape
       strength reduction off leaves -- and neither an indexed
       mBulletPositions[i] loop nor a Vector3 pointer walk (which folds the
       0x37c into the base) gives that register assignment. */
    {
        int n;
        char *ip;
        int k;
        mBasePosX = mPosX;
        n = 0;
        mBasePosY = mPosY;
        ip = ((char*)this);
        mBasePosZ = mPosZ;
        mState = (unsigned char)n;
        mHadClsn = (unsigned char)n;
        k = 0x1cc000;
        do {
            int *py;
            int prod;
            n = n + 1;
            *(int*)(ip+0x37c) = mPosX;
            *(int*)(ip+0x380) = mPosY;
            prod = n * k;
            *(int*)(ip+0x384) = mPosZ;
            py = (int*)(((int)ip + 0x380));
            *py = *py - prod;
            ip = ip + 0xc;
        } while (n < 10);
    }
    return 1;
}
#pragma pop
