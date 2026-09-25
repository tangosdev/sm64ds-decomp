//cpp
/* Production translation unit for ov052/daObjEmmYuka_c.
 * 6 function(s), .text 0x02111440..0x02111830. Tiny-Huge Island's square
 * path lift, actor 175 EMM_YUKA.
 *
 * NAME: _ZTS14daObjEmmYuka_c is "14daObjEmmYuka_c" at ov052 0x021125b8; _ZTI
 * at 0x021125ac reads [__si_class_type_info, that string, _ZTI10dBgActor_c].
 * The tree previously called the class SquarePathLift (coined; vtable address
 * only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02111440), D0
 * (0x02111484), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 *
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange take
 *   Fix12<int> by value, so they stay mangled; a member call homes the
 *   argument and changes the ROM ABI.
 * Leftover: func_020393d4 is a 4-byte store into dBgW's callback slot;
 *   naming belongs with dBgW in arm9.
 * Leftover: the factory daObjEmmYuka_c_classInit (0x02111830) sits just past
 *   this run's right edge and stays a one-function source.
 */

#include "decl_common.h"
#include "daObjEmmYuka_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct EmmYukaFiles {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

extern "C" {
extern EmmYukaFiles data_ov052_021125a0;
void func_020393d4(int *bg, int cb);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int range, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);
void Vec3_Sub(void *out, void *a, void *b);
int LenVec3(void *v);
void Vec3_MulScalar(void *out, void *v, int s);
void SubVec3(void *a, void *b, void *c);
}

namespace cstd { int fdiv(int a, int b); }

#pragma defer_codegen off

// @symbol _ZN14daObjEmmYuka_cD1Ev
// @symbol _ZN14daObjEmmYuka_cD0Ev
daObjEmmYuka_c::~daObjEmmYuka_c()
{
}

// @symbol _ZN14daObjEmmYuka_c16CleanupResourcesEv
int daObjEmmYuka_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov052_021125a0.model->Release();
    data_ov052_021125a0.collision->Release();
    return 1;
}

// @symbol _ZN14daObjEmmYuka_c6RenderEv
int daObjEmmYuka_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjEmmYuka_c8BehaviorEv
int daObjEmmYuka_c::Behavior()
{
    struct Vec { int x, y, z; } prev, node, diff, scaled1, scaled2;
    int looped;
    int delta;
    int len;

    mHorzSpeed = 0xa000;
    delta = mNodeIndex - mPathDir;
    looped = 0;
    if (mPath.Loops()) {
        if (mPathDir > 0) {
            if (delta < 0)
                delta = mPath.NumNodes() - 1;
        } else if (delta >= (int)mPath.NumNodes()) {
            delta = 0;
        }
        mPath.GetNode(*(Vector3 *)&node, delta);
    } else if (delta < 0 || delta >= (int)mPath.NumNodes()) {
        node.x = mPosX;
        node.y = mPosY;
        node.z = mPosZ;
    } else {
        mPath.GetNode(*(Vector3 *)&node, delta);
    }
    mPath.GetNode(*(Vector3 *)&prev, mNodeIndex);
    Vec3_Sub(&diff, &mPosX, &prev);
    len = LenVec3(&diff);
    if (len == 0 || len <= mHorzSpeed) {
        Vec3_MulScalar(&scaled1, &diff, cstd::fdiv(mHorzSpeed, len));
        SubVec3(&mPosX, &scaled1, &mPosX);
        looped = 1;
    } else {
        Vec3_MulScalar(&scaled2, &diff, cstd::fdiv(mHorzSpeed, len));
        SubVec3(&mPosX, &scaled2, &mPosX);
    }
    if (looped) {
        mNodeIndex += mPathDir;
        if (mNodeIndex < 0) {
            if (mPath.Loops())
                mNodeIndex = (int)mPath.NumNodes() - 1;
            else {
                mPathDir = 1;
                mNodeIndex += mPathDir * 2;
            }
        }
        if (mNodeIndex >= (int)mPath.NumNodes()) {
            if (mPath.Loops())
                mNodeIndex = 0;
            else {
                mPathDir = -1;
                mNodeIndex += mPathDir * 2;
            }
        }
    }
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x320000, 0);
    return 1;
}

// @symbol _ZN14daObjEmmYuka_c13InitResourcesEv
int daObjEmmYuka_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov052_021125a0.model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*data_ov052_021125a0.collision),
        &mClsnMat, 0x199, mAngleY, data_ov052_021125a0.clps);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    mPath.FromID(param1 & 0xff);
    mPathDir = 1;
    mHorzSpeed = 0xa000;
    return 1;
}
