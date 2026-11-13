//cpp
/* Lethal Lava Land's seesaw platform (registry profile FL_SEESAW), a
 * dBgActor_c leaf. While mSwingCooldown is zero Behavior adds mSwingStep
 * to mAngleX every frame; once the tilt passes +-0x400 the step's sign
 * flips and the cooldown reloads to 0x1e.
 *
 * _ZTS16daObjFl_Seesaw_c is "16daObjFl_Seesaw_c" at ov022 0x02113ffc and
 * _ZTI names dBgActor_c. The out-of-line destructor is the key function,
 * so this TU emits _ZTV/_ZTI/_ZTS; `#pragma defer_codegen off` lays .text
 * down in source order, so the file is ROM-ascending and the registry
 * factory daObjFl_Seesaw_c_classInit comes last. Do not reorder.
 *
 * Notes:
 * - data_ov022_021145a8 (the seesaw BMD), data_ov022_021145a0 (the KCL)
 *   and data_ov064_0211bacc (the CLPS row SetFile is handed) are unnamed
 *   data rows in their own modules.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjFl_Seesaw_c_AssetId { ASSET_FL_SHISO_BMD = 0x0610, ASSET_FL_SHISO_KCL = 0x0611 };

#include "math/Matrix.h"
#include "daObjFl_Seesaw_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x610, collision file 0x611). mwcc emits
 * __sinit_d_a_obj_fl_seesaw.cpp from those definitions. The wrapper names
 * are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. */
struct SeesawModelFile : SharedFilePtr {
    SeesawModelFile(u32 fileID);
    ~SeesawModelFile();
};

struct SeesawCollisionFilePtr : SharedFilePtr {
    SeesawCollisionFilePtr(u32 fileID);
    ~SeesawCollisionFilePtr();
};

extern SeesawModelFile data_ov022_021145a8;
extern SeesawCollisionFilePtr data_ov022_021145a0;
extern CLPS_Block data_ov064_0211bacc;

#pragma defer_codegen off

/* Vtable slots 16 (D1) and 17 (D0). */
daObjFl_Seesaw_c::~daObjFl_Seesaw_c() {}

/* Writes mModel's transform from mPosX/Y/Z and mAngleX/Y/Z -- the base's
 * UpdateModelPosAndRotY job, on three axes. */
void daObjFl_Seesaw_c::UpdateModelTransform() {
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}

s32 daObjFl_Seesaw_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov022_021145a8.Release();
    data_ov022_021145a0.Release();
    return 1;
}

s32 daObjFl_Seesaw_c::Render() {
    mModel.Render(0);
    return 1;
}

/* Slot 6. */
s32 daObjFl_Seesaw_c::Behavior() {
    mMeshCollider.SetClipRadius(0x650000);
    if (DecIfAbove0_Byte(&mSwingCooldown) == 0) {
        mAngleX += mSwingStep;
        if (mAngleX >= 0x400 || mAngleX <= -0x400) {
            mSwingStep = -mSwingStep;
            mSwingCooldown = 0x1e;
        }
    }
    UpdateModelTransform();
    if (IsClsnInRange(0, 0) != 0) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

/* Slot 0. */
s32 daObjFl_Seesaw_c::InitResources() {
    mModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov022_021145a8), 1, -1);
    UpdateModelTransform();
    UpdateClsnPosAndRot();
    KCL_File *kcl = dBgW_Kc::LoadFile(data_ov022_021145a0);
    mMeshCollider.SetFile(kcl, mClsnMat, 0x1000, mAngleY, data_ov064_0211bacc);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);
    mSwingStep = -0x10;
    return 1;
}

 daObjFl_Seesaw_c *daObjFl_Seesaw_c_classInit() {
    return new daObjFl_Seesaw_c();
}

/* Static-init globals (was the handwritten __sinit_ov022_02112e58 shard).
 * Definition order is the retail initializer's construction order: the
 * model handle (file 0x610), then the collision handle (file 0x611). */
SeesawModelFile data_ov022_021145a8(ASSET_FL_SHISO_BMD);
SeesawCollisionFilePtr data_ov022_021145a0(ASSET_FL_SHISO_KCL);
