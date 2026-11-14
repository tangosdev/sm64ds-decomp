//cpp
/* Tiny-Huge Island's square path lift (actor 175, EMM_YUKA),
 * .text 0x02111440..0x02111830. The out-of-line destructor is the key
 * function, so this TU emits _ZTV/_ZTI/_ZTS14daObjEmmYuka_c: _ZTS is
 * "14daObjEmmYuka_c" at ov052 0x021125b8 and _ZTI at 0x021125ac reads
 * [__si_class_type_info, that string, _ZTI10dBgActor_c]. The tree
 * previously called the class SquarePathLift (coined; vtable address
 * only). #pragma defer_codegen off lays .text down in source order.
 */

// leftovers:
// - dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange take Fix12<int> by
//   value, so they stay mangled; a member call homes the argument and
//   changes the ROM ABI.
// - This TU emits its own __sinit for the file handles below, which the
//   manifest does not license. The handles stay: nothing else defines
//   their BSS or .init range.

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

struct CLPS_Block;

struct EmmYukaFiles {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

/* The ctor/dtor pairs are the ROM's SharedFilePtr veneers (func_02017acc /
 * func_02017ab4 for the model, func_02017b4c / SharedFilePtr_Destruct_Clsn
 * for the collision); the subclasses exist to spell those entry points. */
struct EmmYukaModelFilePtr : SharedFilePtr {
    EmmYukaModelFilePtr(u32 fileID);
    ~EmmYukaModelFilePtr();
};
struct EmmYukaCollisionFilePtr : SharedFilePtr {
    EmmYukaCollisionFilePtr(u32 fileID);
    ~EmmYukaCollisionFilePtr();
};

extern EmmYukaFiles data_ov052_021125a0;
extern "C" {

extern void hold_d_a_obj_emm_yuka(dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
                                  s16 angleY, CLPS_Block *clps);
void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
int VEC_Mag(const Vector3 *v);
void Vec3_MulScalar(Vector3 *out, const Vector3 *v, int s);
/* local extern: CalcLib conflicts with this TU's Vec3_MulScalar spelling. */
void VEC_Subtract(Vector3 *a, Vector3 *b, Vector3 *c);
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
                          mAngleY, *(data_ov052_021125a0.clps));
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
EmmYukaModelFilePtr gEmmYukaModelFile(ASSET_EMM_YUKA_BMD);
EmmYukaCollisionFilePtr gEmmYukaCollisionFile(ASSET_EMM_YUKA_KCL);
