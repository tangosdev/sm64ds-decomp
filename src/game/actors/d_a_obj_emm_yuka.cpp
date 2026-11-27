//cpp
/* Tiny-Huge Island's square path lift (actor 175, EMM_YUKA),
 * .text 0x02111440..0x02111830. The out-of-line destructor is the key
 * function, so this TU emits _ZTV/_ZTI/_ZTS14daObjEmmYuka_c: _ZTS is
 * "14daObjEmmYuka_c" at ov052 0x021125b8 and _ZTI at 0x021125ac reads
 * [__si_class_type_info, that string, _ZTI10dBgActor_c]. The tree
 * previously called the class SquarePathLift (coined; vtable address
 * only). #pragma defer_codegen off lays .text down in source order.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjEmmYuka_c_AssetId {
    ASSET_EMM_YUKA_BMD = 0x05e5,
    ASSET_EMM_YUKA_KCL = 0x05e6
};

#include "decl_common.h"
#include "daObjEmmYuka_c.h"
#include "SharedFilePtr.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"
#include "math/CalcLib.h"

struct CLPS_Block;

struct EmmYukaFiles {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

extern "C" {
extern EmmYukaFiles data_ov052_021125a0;
extern void hold_d_a_obj_emm_yuka(dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
                                  s16 angleY, CLPS_Block *clps);
}

#pragma defer_codegen off

daObjEmmYuka_c::~daObjEmmYuka_c()
{
}

int daObjEmmYuka_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov052_021125a0.model->Release();
    data_ov052_021125a0.collision->Release();
    return 1;
}

int daObjEmmYuka_c::Render()
{
    mModel.Render(0);
    return 1;
}

int daObjEmmYuka_c::Behavior()
{
    Vector3 prev, node, diff, scaled1, scaled2;
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
        mPath.GetNode(node, delta);
    } else if (delta < 0 || delta >= (int)mPath.NumNodes()) {
        node = mPos;
    } else {
        mPath.GetNode(node, delta);
    }
    mPath.GetNode(prev, mNodeIndex);
    Vec3_Sub(&diff, &mPos, &prev);
    len = VEC_Mag(&diff);
    if (len == 0 || len <= mHorzSpeed) {
        Vec3_MulScalar(&scaled1, &diff, FX_Div(mHorzSpeed, len));
        VEC_Subtract(&mPos, &scaled1, &mPos);
        looped = 1;
    } else {
        Vec3_MulScalar(&scaled2, &diff, FX_Div(mHorzSpeed, len));
        VEC_Subtract(&mPos, &scaled2, &mPos);
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
    IsClsnInRange(0x320000, 0);
    return 1;
}

int daObjEmmYuka_c::InitResources()
{
    mModel.SetFile(dExtSimpleModel_c::LoadFile(*data_ov052_021125a0.model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile(dBgW_Kc::LoadFile(*data_ov052_021125a0.collision), mClsnMat, 0x199,
                          mAngle.y, *(data_ov052_021125a0.clps));
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);
    mPath.FromID(param1 & 0xff);
    mPathDir = 1;
    mHorzSpeed = 0xa000;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daObjEmmYuka_c through
 * RTTI, allocation size, vtable identity, and the EMM_YUKA registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: SquarePathLift_Spawn. */
daObjEmmYuka_c *daObjEmmYuka_c_classInit() {
    return new daObjEmmYuka_c();
}

/* The lift's model and collision file handles; data_ov052_021125a0.model
 * and .collision point at them. */
ModelFile gEmmYukaModelFile(ASSET_EMM_YUKA_BMD);
ClsnFile gEmmYukaCollisionFile(ASSET_EMM_YUKA_KCL);

extern CLPS_Block data_ov064_0211ba0c;

EmmYukaFiles data_ov052_021125a0 = {
    &gEmmYukaModelFile,
    &gEmmYukaCollisionFile,
    &data_ov064_0211ba0c,
};
