//cpp
/* Production translation unit for ov065/daObjCtRotateBlock_c.
 * 11 function(s), .text 0x021196d8..0x02119f3c. The rotating cube on Tick
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
 */

// Notes: UpdateShadow is void: its early exit leaves no value, and Behavior
// ignores it.

/* The two registry factories close the run in ROM order:
 * daObjCtRotateBlock_c_classInit_CT_MECHA02 (0x02119ebc, historical alias
 * TtcRotatingPrism_Spawn) and daObjCtRotateBlock_c_classInit_CT_MECHA01
 * (0x02119efc, historical alias TtcRotatingCube_Spawn). Each is operator
 * new(0x3d8), the dBgActor_c constructor, the vtable store and the dExtSimpleModel_c and
 * dExtShadowModel_c constructors, which is exactly `new daObjCtRotateBlock_c()`.
 * Their names are reconstructed from the profiles; retail does not store
 * them. The new-expression also emits dBgActor_c's base-object destructor,
 * which has no ROM home (manifest: deadstrip).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjCtRotateBlock_c_AssetId {
    ASSET_CT_MECHA_OBJ01_BMD = 0x05b6,
    ASSET_CT_MECHA_OBJ01_KCL = 0x05b7,
    ASSET_CT_MECHA_OBJ01A_BMD = 0x05b8,
    ASSET_CT_MECHA_OBJ02_BMD = 0x05b9,
    ASSET_CT_MECHA_OBJ02_KCL = 0x05ba,
    ASSET_CT_MECHA_OBJ02A_BMD = 0x05bb
};

#include "decl_common.h"
#include "daObjCtRotateBlock_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "LevelRng.h"
#include "nitro/fx/fx_trig.h"

/* One record per mVariant: the base model's and the collision mesh's file
 * handles, and the CLPS block SetFile gets with that mesh. */
struct CtRotateBlockFiles {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
extern CtRotateBlockFiles data_ov065_0211cfd0[2];

/* File-scope objects at the end of this file construct the six resource
 * handles (model files 0x5b6/0x5b9/0x5b8/0x5bb, collision files 0x5b7/
 * 0x5ba). mwcc emits __sinit_d_a_obj_ct_rotate_block.cpp from those
 * definitions. The wrapper names are local; the handle constructors and
 * destructors are the ROM resource-family functions, aliased in the
 * manifest. Nothing in this TU names the handles directly; the variant
 * records above point at them. */
struct RotateBlockModelFile : SharedFilePtr {
    RotateBlockModelFile(u32 fileID);
    ~RotateBlockModelFile();
};

struct RotateBlockCollisionFilePtr : SharedFilePtr {
    RotateBlockCollisionFilePtr(u32 fileID);
    ~RotateBlockCollisionFilePtr();
};

extern u8 data_0209f2c0;

#pragma defer_codegen off

daObjCtRotateBlock_c::~daObjCtRotateBlock_c() {}

void daObjCtRotateBlock_c::UpdateShadow() {
    int d, b;
    int bval = mFloorY;
    int aval = mPosY;
    int diff = aval - bval;
    if (diff < 0)
        diff = -diff;
    if (diff > 0x9c4000)
        return;
    b = 0x32000;
    d = 0x190000;
    if (mUnevenGround) {
        b = 0x17c000;
        d = b;
    }
    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = (mFloorY + 0x1000) >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    {
        int s = FX_SinIdx((u16)(s16)(mAngleZ << 1) >> 4);
        int sa = s < 0 ? -s : s;
        int depthAdd = FX_MUL(sa, 0x64000);
        DropShadowScaleXYZ(mShadowModel, mShadowMat, d + depthAdd, b, d, 0xf);
    }
}

void daObjCtRotateBlock_c::UpdateClsn() {
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9] = mPosX;
    mClsnMat.m[10] = mPosY + mOffsetY;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
}

void daObjCtRotateBlock_c::UpdateModel() {
    Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, 0, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = (mPosY + mOffsetY) >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
    Matrix4x3_FromRotationY(&mRotatingModel.mat4x3, mAngleY);
    mRotatingModel.mat4x3.m[9] = mModel.mat4x3.m[9];
    mRotatingModel.mat4x3.m[10] = mPosY >> 3;
    mRotatingModel.mat4x3.m[11] = mModel.mat4x3.m[11];
}

int daObjCtRotateBlock_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)data_ov065_0211c0a8[mVariant])->Release();
    data_ov065_0211cfd0[mVariant].model->Release();
    data_ov065_0211cfd0[mVariant].collision->Release();
    return 1;
}

int daObjCtRotateBlock_c::Render() {
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

int daObjCtRotateBlock_c::Behavior() {
    if (data_0209f2c0 != 3) {
        switch (mState) {
        case 0:
            if (DecIfAbove0_Short(&mWaitTimer) != 0)
                break;
            Sound::PlayBank3(0x5b, mCamSpacePos);
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
            Sound::PlayBank3(0x40, mCamSpacePos);
            mState = 0;
            mWaitTimer = data_ov065_0211cfa4[data_0209f2c0];
            if (data_0209f2c0 == 2)
                mWaitTimer = (unsigned int)RandomIntInternal(&data_0209e650.value) % 7 * 0x14 + 5;
            mTargetAngleZ += data_ov065_0211cfa8[mVariant];
            break;
        }
    }
    UpdateModel();
    UpdateShadow();
    if (IsClsnInRange(0, 0))
        UpdateClsn();
    return 1;
}

int daObjCtRotateBlock_c::InitResources() {
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
        BMD_File *bmd = dExtSimpleModel_c::LoadFile(*data_ov065_0211cfd0[idx].model);
        mModel.SetFile(bmd, 1, -1);
    }

    {
        BMD_File *bmd =
            dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)data_ov065_0211c0a8[mVariant]);
        mRotatingModel.SetFile(bmd, 1, -1);
    }

    mShadowModel.InitCuboid();

    mTargetAngleZ = data_ov065_0211cfa8[mVariant];

    UpdateModel();
    UpdateClsn();

    idx = mVariant;
    if (idx == 0) {
        KCL_File *kcl = dBgW_Kc::LoadFile(*data_ov065_0211cfd0[idx].collision);
        mMeshCollider.SetFile(kcl, mClsnMat, 0x1000, mAngleY, *data_ov065_0211cfd0[idx].clps);
    } else {
        KCL_File *kcl = dBgW_Kc::LoadFile(*data_ov065_0211cfd0[idx].collision);
        mMeshCollider.SetFile(kcl, mClsnMat, 0x199, mAngleY, *data_ov065_0211cfd0[idx].clps);
    }

    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);

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

        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MTX_MultVec43(&va, &data_020a0e68, &vb);
        VEC_Add(&vb, &mPos, &vb);
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
        s32 probeFloor = vb.y;
        if (ground.DetectClsn() != 0) {
            probeFloor = ground.clsnY;
        }
        if (probeFloor != mFloorY) {
            mUnevenGround = 1;
        }
    }

    return 1;
}

extern "C" daObjCtRotateBlock_c *daObjCtRotateBlock_c_classInit_CT_MECHA02() {
    return new daObjCtRotateBlock_c();
}

extern "C" daObjCtRotateBlock_c *daObjCtRotateBlock_c_classInit_CT_MECHA01() {
    return new daObjCtRotateBlock_c();
}

/* Static-init globals (was the handwritten __sinit_ov065_0211c660 shard).
 * Definition order is the retail initializer's construction order. */
RotateBlockModelFile gCtMechaObj01ModelFile(ASSET_CT_MECHA_OBJ01_BMD);
RotateBlockModelFile gCtMechaObj02ModelFile(ASSET_CT_MECHA_OBJ02_BMD);
RotateBlockModelFile gCtMechaObj01aModelFile(ASSET_CT_MECHA_OBJ01A_BMD);
RotateBlockModelFile gCtMechaObj02aModelFile(ASSET_CT_MECHA_OBJ02A_BMD);
RotateBlockCollisionFilePtr gCtMechaObj01CollisionFile(ASSET_CT_MECHA_OBJ01_KCL);
RotateBlockCollisionFilePtr gCtMechaObj02CollisionFile(ASSET_CT_MECHA_OBJ02_KCL);
