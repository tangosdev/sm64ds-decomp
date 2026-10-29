//cpp
/**
 * Rainbow Ride flying carpet (ov036 FLYING_CARPET 130).
 *
 * RTTI ov036:0x02113f4c names daObjRcCarpet_c; overlay_actors maps
 * profile 130 to FLYING_CARPET. ov036 is RR. Not buranko / hane /
 * guruguru (siblings in this overlay).
 *
 * Notes:
 * - classInit is `return new daObjRcCarpet_c()`; dPathLiftActor_c's
 *   implicit constructor inlines into it, as in d_a_obj_path_lift.cpp.
 * - func_020393d4 is an 8-byte store into dBgW+0x18 (beforeClsnCallback);
 *   dBgW.h has no setter. Matrix4x3_FromRotationXYZExt has no header either.
 * - data_ov002_0210d9f0 and data_ov036_02112b28 CLPS stay outside this TU.
 *   The three ov036 handles are file-scope objects at the bottom of this
 *   file.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_rc_carpet_AssetId {
    ASSET_RC_CARPET_BMD = 0x0697,
    ASSET_RC_CARPET_KCL = 0x0698,
    ASSET_RC_CARPET_WAIT_BCA = 0x0699
};

#include "common.h"
#include "daObjRcCarpet_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "math/Approach.h"

struct FlyingCarpetResources {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

typedef char
    FlyingCarpetResources_size_must_be_0x0c[sizeof(FlyingCarpetResources) == 0x0c ? 1 : -1];

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct CarpetModelFilePtr : SharedFilePtr {
    CarpetModelFilePtr(u32 fileID);
    ~CarpetModelFilePtr();
};

struct CarpetAnimationFilePtr : SharedFilePtr {
    CarpetAnimationFilePtr(u32 fileID);
    ~CarpetAnimationFilePtr();
};

struct CarpetCollisionFilePtr : SharedFilePtr {
    CarpetCollisionFilePtr(u32 fileID);
    ~CarpetCollisionFilePtr();
};

extern SharedFilePtr data_ov002_0210d9f0;
extern CarpetModelFilePtr gRcCarpetModelFile;
extern CarpetAnimationFilePtr gRcCarpetWaitAnimationFile;
extern CarpetCollisionFilePtr gRcCarpetCollisionFile;
extern CLPS_Block data_ov036_02112b28;

/* Retail resource-pointer table at ov036:0x02113f58. */
FlyingCarpetResources data_ov036_02113f58 = {&gRcCarpetModelFile, &gRcCarpetCollisionFile,
                                             &data_ov036_02112b28};

extern "C" {
void func_020393d4(dBgW *collider, void *callback);
void Matrix4x3_FromRotationXYZExt(Matrix4x3 *matrix, s16 x, s16 y, s16 z);
}

// @symbol daObjRcCarpet_c_classInit
extern "C" daObjRcCarpet_c *daObjRcCarpet_c_classInit() {
    return new daObjRcCarpet_c();
}

extern "C" SpawnInfo<daObjRcCarpet_c> g_profile_RC_CARPET = {
    daObjRcCarpet_c_classInit,
    0x0082,
    0x0104,
    0x00000002,
    0x00000000,
    0x00300000,
    0x01000000,
    0x00000000,
};

// @symbol _ZN15daObjRcCarpet_c13InitResourcesEv
int daObjRcCarpet_c::InitResources() {
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9f0);
    void *modelFile = dExtSimpleModel_c::LoadFile(*data_ov036_02113f58.model);
    mAnmModel.SetFile((BMD_File *)modelFile, 1, -1);
    dExtFrameCtrl_c::LoadFile(gRcCarpetWaitAnimationFile);
    mAnmModel.SetAnim((BCA_File *)gRcCarpetWaitAnimationFile.filePtr, 0, 0x1000, 0);
    UpdateModelTransform();
    UpdateClsnPosAndRot();
    KCL_File *collisionFile = (KCL_File *)dBgW_Kc::LoadFile(*data_ov036_02113f58.collision);
    mMeshCollider.SetFile(collisionFile, mClsnMat, 0x199, mAngleY, *data_ov036_02113f58.clps);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);
    mPathSpeed = 0xa000;
    mHorzSpeed = mPathSpeed;
    BaseInitResources();
    mPathDirection = 1;
    mRespawnAfterFall = 1;
    return 1;
}

// @symbol _ZN15daObjRcCarpet_c8BehaviorEv
int daObjRcCarpet_c::Behavior() {
    if (mAfterClsnRan)
        ApproachLinear(mRideDipY, -0x14000, 0x5000);
    else
        ApproachLinear(mRideDipY, 0, 0x5000);
    BaseBehavior();
    mAnmModel.Advance();
    UpdateModelTransform();
    UpdateClsnTransform();
    IsClsnInRange(0, 0);
    return 1;
}

// @symbol _ZN15daObjRcCarpet_c6RenderEv
int daObjRcCarpet_c::Render() {
    if (mWaitTimer < 0x5a && (mWaitTimer & 1))
        return 1;
    mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjRcCarpet_c16CleanupResourcesEv
int daObjRcCarpet_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov002_0210d9f0.Release();
    data_ov036_02113f58.model->Release();
    data_ov036_02113f58.collision->Release();
    gRcCarpetWaitAnimationFile.Release();
    return 1;
}

// @symbol _ZN15daObjRcCarpet_c20UpdateModelTransformEv
void daObjRcCarpet_c::UpdateModelTransform() {
    Matrix4x3_FromRotationXYZExt(&mAnmModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = (mPosY + mRideDipY) >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN15daObjRcCarpet_c19UpdateClsnTransformEv
void daObjRcCarpet_c::UpdateClsnTransform() {
    mClsnMat = mAnmModel.mat4x3;
    mClsnMat.m[9] = mPosX;
    mClsnMat.m[10] = mPosY + mRideDipY;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
}
/* ROM ordinals 0/1 are emitted by the inline virtual destructor:*/
// @symbol _ZN15daObjRcCarpet_cD1Ev
// @symbol _ZN15daObjRcCarpet_cD0Ev

CarpetModelFilePtr gRcCarpetModelFile(ASSET_RC_CARPET_BMD);
CarpetAnimationFilePtr gRcCarpetWaitAnimationFile(ASSET_RC_CARPET_WAIT_BCA);
CarpetCollisionFilePtr gRcCarpetCollisionFile(ASSET_RC_CARPET_KCL);
