//cpp
/**
 * Lethal Lava Land lava bridge -- ov022/daObjFl_London_c (8 functions).
 *
 * ROM run 0x02111980..0x02111cac, plus the class's .data run at
 * 0x02113f2c..0x02113ff0 (_ZTI, _ZTS, g_profile_FL_LONDON, _ZTV). The run
 * ends where _ZTI16daObjFl_Seesaw_c begins.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the reverse of source order.
 *
 * daObjFl_London_c_classInit is reconstructed (RTTI daObjFl_London_c,
 * LAVA_BRIDGE / FL_LONDON registry). Retail does not store that spelling.
 *
 * What stays, and why:
 * - func_020393d4 stores &dBgW::UpdatePosWithTransform (no setter).
 * - data_ov022_02114580 / data_ov022_02114578 SharedFilePtr handles
 *   and data_ov064_0211bb2c CLPS; symbols.txt has no recovered names.
 * - common.h first (dBgActor_c.h already pulls it; dExtSimpleModel_c.h's
 *   nested Matrix4x3 would scalarize a 12-word copy).
 * - Behavior writes mPrevAngleZ through a short* (member assignment
 *   size-DIFFs).
 * - Inline destructor (out-of-line emits D0 before D1 plus a homeless D2).
 */

#include "common.h"
#include "daObjFl_London_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "dBgW.h"
#include "dBgW_Kc.h"
#include "math/CalcLib.h"

struct CLPS_Block;

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x5fd, collision file 0x5fe). mwcc emits
 * __sinit_d_a_obj_fl_london.cpp from those definitions. The wrapper names
 * are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. */
struct LondonModelFile : SharedFilePtr {
    LondonModelFile(u32 fileID);
    ~LondonModelFile();
};

struct LondonCollisionFilePtr : SharedFilePtr {
    LondonCollisionFilePtr(u32 fileID);
    ~LondonCollisionFilePtr();
};

extern LondonModelFile data_ov022_02114580;
extern LondonCollisionFilePtr data_ov022_02114578;
extern CLPS_Block data_ov064_0211bb2c;

/* local extern: the arm9 rotation builder and the dBgW callback store,
 * with no shared header yet. */
extern "C" {
void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
void func_020393d4(void *p, void *v);
}

// @symbol daObjFl_London_c_classInit
extern "C" daObjFl_London_c *daObjFl_London_c_classInit() {
    return new daObjFl_London_c();
}

/* The actor profile the registry points at. Word 0 relocates to the
 * factory above; the rest is the ROM's, read back at 0x02113f4c. */
// @symbol g_profile_FL_LONDON
extern "C" SpawnInfo<daObjFl_London_c> g_profile_FL_LONDON = {
    daObjFl_London_c_classInit,
    0x0049,
    0x0119,
    0x00000002,
    0x00100000,
    0x00300000,
    0x02000000,
    0x00000000,
};

// @symbol _ZN16daObjFl_London_c13InitResourcesEv
s32 daObjFl_London_c::InitResources() {
    void *f = dExtSimpleModel_c::LoadFile(data_ov022_02114580);
    mModel.SetFile((BMD_File *)f, 1, -1);
    UpdateModelMatrix();
    UpdateClsnPosAndRot();
    void *k = dBgW_Kc::LoadFile(data_ov022_02114578);
    mMeshCollider.SetFile((KCL_File *)k, mClsnMat, 0x1000, mAngleY, data_ov064_0211bb2c);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    mFlag = 0;
    mCooldown = 0xf;
    return 1;
}

// @symbol _ZN16daObjFl_London_c8BehaviorEv
s32 daObjFl_London_c::Behavior() {
    if (DecIfAbove0_Byte(&mCooldown) == 0) {
        if (mFlag == 0) {
            /* MATCH: mPrevAngleZ = mPrevAngleZ - 0x100 size-DIFFs (member
             * addressing CSEs the field). */
            short *p = &mPrevAngleZ;
            *p = *p - 0x100;
            if (mPrevAngleZ <= -0x2000) {
                mPrevAngleZ = -0x2000;
                mCooldown = 0xf;
                mFlag = 1;
            }
        } else {
            short *p = &mPrevAngleZ;
            *p = *p + 0x100;
            if (mPrevAngleZ >= 0) {
                mPrevAngleZ = 0;
                mCooldown = 0xf;
                mFlag = 0;
            }
        }
    } else {
        if (mCooldown == 1) {
            if (mFlag == 0)
                Sound::PlayBank3(0x34, mCamSpacePos);
            else
                Sound::PlayBank3(0x35, mCamSpacePos);
        }
    }
    mAngleZ = mPrevAngleZ;
    UpdateModelMatrix();
    if (IsClsnInRange(0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN16daObjFl_London_c6RenderEv
s32 daObjFl_London_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjFl_London_c16CleanupResourcesEv
s32 daObjFl_London_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov022_02114580.Release();
    data_ov022_02114578.Release();
    return 1;
}

// @symbol _ZN16daObjFl_London_c17UpdateModelMatrixEv
/* Rebuild the model matrix from the actor's three angles, then publish the
 * position at 1/8 scale. Both callers (InitResources and Behavior) are in
 * this TU. */
void daObjFl_London_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN16daObjFl_London_cD1Ev
// @symbol _ZN16daObjFl_London_cD0Ev
/* Both destructors are emitted from the INLINE `~daObjFl_London_c() {}` in
 * include/daObjFl_London_c.h -- there is deliberately no body here.
 *
 * Defining ~daObjFl_London_c() out of line HERE instead would break the TU two
 * ways: mwccarm would emit D0 at 0x021119c4 before D1 at 0x02111980, reversing
 * ROM order so objisolate refuses the entire TU, and it would emit a third
 * symbol, D2, which has no address anywhere in the cartridge.
 */

/* Static-init globals (was the handwritten __sinit_ov022_02112dec shard).
 * Definition order is the retail initializer's construction order: the
 * model handle (file 0x5fd), then the collision handle (file 0x5fe). */
LondonModelFile data_ov022_02114580(0x5fd);
LondonCollisionFilePtr data_ov022_02114578(0x5fe);
