//cpp
/* Production translation unit for ov065/daObjCtRotateBlock_c.
 * 9 function(s), .text 0x021196d8..0x02119ebc. The rotating cube on Tick
 * Tock Clock (profiles CT_MECHA01 and CT_MECHA02).
 *
 * NAME: _ZTS20daObjCtRotateBlock_c is "20daObjCtRotateBlock_c" at ov065
 * 0x0211cfb8; _ZTI at 0x0211cfac reads [__si_class_type_info, that string,
 * _ZTI10dBgActor_c]. The tree previously called the class TtcRotatingCube
 * (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x021196d8), D0
 * (0x0211972c), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 *
 * Leftover: dBgW_KcMbg::SetFile, dActor_c::DropShadowScaleXYZ and
 *   dBgActor_c::IsClsnInRange take Fix12<int> by value, so they stay mangled;
 *   a member call homes the argument and changes the ROM ABI.
 * Leftover: func_020393d4 is a 4-byte store into dBgW's callback slot;
 *   naming belongs with dBgW in arm9.
 * Leftover: the two factories, daObjCtRotateBlock_c_classInit_CT_MECHA02
 *   (0x02119ebc) and _CT_MECHA01 (0x02119efc), sit past this run's right
 *   edge and stay one-function sources.
 */

#include "decl_common.h"
#include "daObjCtRotateBlock_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"

/* Per-variant records are 0xc bytes. The first word is a SharedFilePtr. */
extern char data_ov065_0211cfd0[];
extern char data_ov065_0211cfd4[];

extern "C" {
extern void Matrix4x3_FromRotationY(void *m, short ang);
extern void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
extern void MulVec3Mat4x3(void *src, void *mtx, void *dst);
extern void AddVec3(void *a, void *b, void *c);
extern u16 DecIfAbove0_Short(u16 *p);
extern int RandomIntInternal(int *seed);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern int _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *mc, void *kcl, void *mtx, int fix, s16 s, void *clps);
extern void func_020393d4(void *p, void *v);
extern short data_02082214[];
extern u8 data_0209f2c0;
extern int data_0209e650;
extern s32 data_020a0e68[];
}

int ApproachLinear(s16 &val, short target, short step);

#pragma defer_codegen off

// @symbol _ZN20daObjCtRotateBlock_cD1Ev
// @symbol _ZN20daObjCtRotateBlock_cD0Ev
daObjCtRotateBlock_c::~daObjCtRotateBlock_c()
{
}

// @symbol _ZN20daObjCtRotateBlock_c12UpdateShadowEv
int daObjCtRotateBlock_c::UpdateShadow()
{
    int d, b;
    int bval = mFloorY;
    int aval = mPosY;
    int diff = aval - bval;
    if (diff < 0) diff = -diff;
    if (diff > 0x9c4000) return 0x9c4000;
    b = 0x32000;
    d = 0x190000;
    if (mUnevenGround) { b = 0x17c000; d = b; }
    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = (mFloorY + 0x1000) >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    {
        int idx = (unsigned short)(short)(mAngleZ << 1) >> 4;
        int s = data_02082214[idx << 1];
        int sa = s < 0 ? -s : s;
        int r3 = (int)(((long long)sa * 0x64000 + 0x800) >> 12);
        return _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            this, &mShadowModel, &mShadowMat, d + r3, b, d, 0xf);
    }
}

// @symbol _ZN20daObjCtRotateBlock_c10UpdateClsnEv
void daObjCtRotateBlock_c::UpdateClsn()
{
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9] = mPosX;
    mClsnMat.m[10] = mPosY + mOffsetY;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
}

// @symbol _ZN20daObjCtRotateBlock_c11UpdateModelEv
void daObjCtRotateBlock_c::UpdateModel()
{
    Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, 0, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = (mPosY + mOffsetY) >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
    Matrix4x3_FromRotationY(&mRotatingModel.mat4x3, mAngleY);
    mRotatingModel.mat4x3.m[9] = mModel.mat4x3.m[9];
    mRotatingModel.mat4x3.m[10] = mPosY >> 3;
    mRotatingModel.mat4x3.m[11] = mModel.mat4x3.m[11];
}

// @symbol _ZN20daObjCtRotateBlock_c16CleanupResourcesEv
int daObjCtRotateBlock_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)data_ov065_0211c0a8[mVariant])->Release();
    ((SharedFilePtr *)*(void **)(data_ov065_0211cfd0 + mVariant * 0xc))->Release();
    ((SharedFilePtr *)*(void **)(data_ov065_0211cfd4 + mVariant * 0xc))->Release();
    return 1;
}

// @symbol _ZN20daObjCtRotateBlock_c6RenderEv
int daObjCtRotateBlock_c::Render()
{
    int flags;
    int b;
    flags = mFlags;
    b = flags & 8;
    b = (b != 0);
    if (!b) {
        mModel.Render(0);
        mRotatingModel.Render(0);
    }
    return 1;
}

// @symbol _ZN20daObjCtRotateBlock_c8BehaviorEv
int daObjCtRotateBlock_c::Behavior()
{
    if (data_0209f2c0 != 3) {
        switch (mState) {
        case 0:
            if (DecIfAbove0_Short(&mWaitTimer) != 0)
                break;
            Sound::PlayBank3(0x5b, *(Vector3 *)&mCamSpacePosX);
            mState++;
            mVertSpeed = -0x5000;
            break;
        case 1:
            mVertSpeed += 0x800;
            mOffsetY += mVertSpeed;
            if (mOffsetY < 0)
                break;
            mOffsetY = 0;
            mWaitTimer = 6;
            mState++;
            break;
        case 2:
            if (DecIfAbove0_Short(&mWaitTimer) != 0)
                break;
            if (ApproachLinear(mAngleZ, mTargetAngleZ, 0x4b0) == 0)
                break;
            Sound::PlayBank3(0x40, *(Vector3 *)&mCamSpacePosX);
            mState = 0;
            mWaitTimer = data_ov065_0211cfa4[data_0209f2c0];
            if (data_0209f2c0 == 2)
                mWaitTimer = (unsigned int)RandomIntInternal(&data_0209e650) % 7 * 0x14 + 5;
            mTargetAngleZ += data_ov065_0211cfa8[mVariant];
            break;
        }
    }
    UpdateModel();
    UpdateShadow();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsn();
    return 1;
}

// @symbol _ZN20daObjCtRotateBlock_c13InitResourcesEv
int daObjCtRotateBlock_c::InitResources()
{
    u16 id = actorID;

    if (id != 0x6c) {
        if (id == 0x6d) {
            mVariant = 1;
        }
    } else {
        mVariant = 0;
    }

    u8 idx = mVariant;
    {
        void *bmd = Model::LoadFile(*(SharedFilePtr *)*(void **)(data_ov065_0211cfd0 + idx * 0xc));
        mModel.SetFile((BMD_File *)bmd, 1, -1);
    }

    {
        void *bmd = Model::LoadFile(*(SharedFilePtr *)data_ov065_0211c0a8[mVariant]);
        mRotatingModel.SetFile((BMD_File *)bmd, 1, -1);
    }

    mShadowModel.InitCuboid();

    mTargetAngleZ = data_ov065_0211cfa8[mVariant];

    UpdateModel();
    UpdateClsn();

    idx = mVariant;
    if (idx == 0) {
        s32 oi = idx * 0xc;
        void *kcl = dBgW_Kc::LoadFile(*(SharedFilePtr *)*(void **)(data_ov065_0211cfd4 + oi));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
            *(void **)((char *)data_ov065_0211cfd8 + oi));
    } else {
        s32 oi = idx * 0xc;
        void *kcl = dBgW_Kc::LoadFile(*(SharedFilePtr *)*(void **)(data_ov065_0211cfd4 + oi));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
            *(void **)((char *)data_ov065_0211cfd8 + oi));
    }

    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);

    {
        Vector3 va;
        Vector3 vb;
        mWaitTimer = data_ov065_0211cfa4[data_0209f2c0];
        va.x = 0;
        va.z = 0;
        vb.x = 0;
        vb.y = 0;
        vb.z = 0;
        va.y = 0;
        va.x = 0x64000;
        va.z = 0x64000;

        Matrix4x3_FromRotationY(data_020a0e68, mAngleY);
        MulVec3Mat4x3(&va, data_020a0e68, &vb);
        AddVec3(&vb, &mPosX, &vb);
        vb.y -= 0xd2000;

        dBgCh_Gnd ground;
        ground.SetObjAndPos(vb, 0);
        mFloorY = vb.y;
        if (ground.DetectClsn() != 0) {
            mFloorY = ground.clsnY;
        }

        vb.x = mPosX;
        vb.y = mPosY;
        vb.z = mPosZ;
        vb.y -= 0xd2000;

        ground.SetObjAndPos(vb, 0);
        s32 r5 = vb.y;
        if (ground.DetectClsn() != 0) {
            r5 = ground.clsnY;
        }
        if (r5 != mFloorY) {
            mUnevenGround = 1;
        }
    }

    return 1;
}
