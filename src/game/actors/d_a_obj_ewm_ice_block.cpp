//cpp
/* The big ice block that moves along a path (registry profile EWM_ICE_BLOCK),
 * .text 0x021111a0..0x021115cc. The out-of-line destructor is the key
 * function, so this TU emits _ZTV/_ZTI/_ZTS18daObjEwmIceBlock_c: _ZTS is
 * "18daObjEwmIceBlock_c" at ov056 0x0211332c and _ZTI at 0x02113320 reads
 * [__si_class_type_info, that string, _ZTI10dBgActor_c]. The tree
 * previously called the class BigMovingIceBlock (coined). #pragma
 * defer_codegen off lays .text down in source order, and the classInit
 * factory appends after InitResources. The factory name is reconstructed
 * (historical alias BigMovingIceBlock_Spawn); retail does not store it.
 */

// leftovers:
// - dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange take Fix12<int> by
//   value, so they stay mangled; a member call homes the argument and
//   changes the ROM ABI.
// - func_020393d4 is the 4-byte store of the collision callback into
//   dBgW's slot; naming belongs with dBgW in arm9.
// - This TU emits its own __sinit for the file handles below, which the
//   manifest does not license. The handles stay: nothing else defines
//   their BSS or .init range.
// - q stays branch-scoped: hoisting it renumbers the static-init
//   temporaries (@490/@491), which the manifest pins.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjEwmIceBlock_c_AssetId {
    ASSET_EWM_ICE_BROCK_BMD = 0x05ed,
    ASSET_EWM_ICE_BROCK_KCL = 0x05ee
};

#include "decl_common.h"
#include "daObjEwmIceBlock_c.h"
#include "SharedFilePtr.h"

namespace cstd {
int fdiv(int a, int b);
}

extern "C" {
extern SharedFilePtr *data_ov056_02113314[];
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern int LenVec3(Vector3 *v);
extern void Vec3_MulScalar(Vector3 *out, Vector3 *v, int s);
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void hold_d_a_obj_ewm_ice_block(dBgW_KcMbg *self, void *kcl, const Matrix4x3 *mtx, int scale,
                                       short angle, void *clps);
extern void func_020393d4(void *p, void *cb);
}

#pragma defer_codegen off

// @symbol _ZN18daObjEwmIceBlock_cD1Ev
// @symbol _ZN18daObjEwmIceBlock_cD0Ev
daObjEwmIceBlock_c::~daObjEwmIceBlock_c()
{
}

// @symbol _ZN18daObjEwmIceBlock_c16CleanupResourcesEv
int daObjEwmIceBlock_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov056_02113314[0]->Release();
    data_ov056_02113314[1]->Release();
    return 1;
}

// @symbol _ZN18daObjEwmIceBlock_c6RenderEv
int daObjEwmIceBlock_c::Render()
{
    mModel.Render((const Vector3 *)0);
    return 1;
}

// @symbol _ZN18daObjEwmIceBlock_c8BehaviorEv
int daObjEwmIceBlock_c::Behavior()
{
    Vector3 prev;
    Vector3 node;
    Vector3 diff;
    Vector3 scaled1;
    Vector3 scaled2;
    int looped;
    int delta;
    int len;

    mHorzSpeed = 0xa000;
    delta = mPathNodeIdx - mPathDir;
    looped = 0;

    if (mPath.Loops()) {
        if (mPathDir > 0) {
            if (delta < 0)
                delta = (int)mPath.NumNodes() - 1;
        } else {
            if (delta >= (int)mPath.NumNodes())
                delta = 0;
        }
        mPath.GetNode(node, delta);
    } else {
        if (delta < 0 || delta >= (int)mPath.NumNodes()) {
            node.x = mPosX;
            node.y = mPosY;
            node.z = mPosZ;
        } else {
            mPath.GetNode(node, delta);
        }
    }

    mPath.GetNode(prev, mPathNodeIdx);
    Vec3_Sub(&diff, &mPos, &prev);
    len = LenVec3(&diff);
    if (len == 0 || len <= mHorzSpeed) {
        int q = cstd::fdiv(mHorzSpeed, len);
        Vec3_MulScalar(&scaled1, &diff, q);
        SubVec3(&mPos, &scaled1, &mPos);
        looped = 1;
    } else {
        int q = cstd::fdiv(mHorzSpeed, len);
        Vec3_MulScalar(&scaled2, &diff, q);
        SubVec3(&mPos, &scaled2, &mPos);
    }

    if (looped) {
        mPathNodeIdx += mPathDir;
        if (mPathNodeIdx < 0) {
            if (mPath.Loops())
                mPathNodeIdx = (int)mPath.NumNodes() - 1;
            else {
                mPathDir = 1;
                mPathNodeIdx += mPathDir * 2;
            }
        }
        if (mPathNodeIdx >= (int)mPath.NumNodes()) {
            if (mPath.Loops())
                mPathNodeIdx = 0;
            else {
                mPathDir = -1;
                mPathNodeIdx += mPathDir * 2;
            }
        }
    }

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    IsClsnInRange(0x5dc000, 0);
    return 1;
}

// @symbol _ZN18daObjEwmIceBlock_c13InitResourcesEv
int daObjEwmIceBlock_c::InitResources()
{
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov056_02113314[0]), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile((KCL_File *)(dBgW_Kc::LoadFile(*data_ov056_02113314[1])),
                              *(&mClsnMat), 0x199, mAngleY,
                              *(CLPS_Block *)(data_ov056_02113314[2]));
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    mPath.FromID(param1 & 0xff);
    mPathDir = 1;
    mHorzSpeed = 0xa000;
    return 1;
}

// @symbol daObjEwmIceBlock_c_classInit
extern "C" daObjEwmIceBlock_c *daObjEwmIceBlock_c_classInit()
{
    return new daObjEwmIceBlock_c();
}

/* The ctor/dtor pairs are the ROM's SharedFilePtr veneers (func_02017acc /
 * func_02017ab4 for the model, func_02017b4c / SharedFilePtr_Destruct_Clsn
 * for the collision); the subclasses exist to spell those entry points. */
struct EwmIceBlockModelFilePtr : SharedFilePtr {
    EwmIceBlockModelFilePtr(u32 fileID);
    ~EwmIceBlockModelFilePtr();
};
struct EwmIceBlockCollisionFilePtr : SharedFilePtr {
    EwmIceBlockCollisionFilePtr(u32 fileID);
    ~EwmIceBlockCollisionFilePtr();
};

// @symbol __sinit_d_a_obj_ewm_ice_block.cpp
EwmIceBlockModelFilePtr gEwmIceBrockModelFile(ASSET_EWM_ICE_BROCK_BMD);      /* model */
EwmIceBlockCollisionFilePtr gEwmIceBrockCollisionFile(ASSET_EWM_ICE_BROCK_KCL);  /* collision */
