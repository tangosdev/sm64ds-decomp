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
 * leftovers:
 * - dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay extern "C"
 *   scalar spellings: the real members take Fix12<int> by value, which
 *   the bytes refuse (the Fix12 wall; daObjFl_Block_c carries the same
 *   note).
 * - func_020393a4 and func_020393d4 stay extern "C": they store into dBgW
 *   internals and have no member form.
 * - data_ov022_021145a8 (the seesaw BMD), data_ov022_021145a0 (the KCL)
 *   and data_ov064_0211bacc (the CLPS row SetFile is handed) are unnamed
 *   data rows in their own modules.
 */

#include "math/Matrix.h"
#include "daObjFl_Seesaw_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
extern SharedFilePtr data_ov022_021145a8;
extern SharedFilePtr data_ov022_021145a0;
extern char data_ov064_0211bacc[];

void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
unsigned char DecIfAbove0_Byte(unsigned char *p);

/* Scalar ABI. The real members take Fix12<int> by value. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, char *kcl, const Matrix4x3 *mtx, int scale, s16 angleY,
    char *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int radius, int yOffset);

/* Stores at dBgW+0x0c and +0x1c. Spellings match the C definitions. */
void func_020393a4(int *mesh, int range);
void func_020393d4(int *mesh, int callback);
}

#pragma defer_codegen off

/* Vtable slots 16 (D1) and 17 (D0). */
// @symbol _ZN16daObjFl_Seesaw_cD1Ev
// @symbol _ZN16daObjFl_Seesaw_cD0Ev
daObjFl_Seesaw_c::~daObjFl_Seesaw_c()
{
}

/* Writes mModel's transform from mPosX/Y/Z and mAngleX/Y/Z -- the base's
 * UpdateModelPosAndRotY job, on three axes. r0 is the object and the body
 * only touches this class's members, so the function is a member and the
 * parameter is this; the address is kept as the method name. */
// @symbol _ZN16daObjFl_Seesaw_c19func_ov022_02111d48Ev
void daObjFl_Seesaw_c::func_ov022_02111d48()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}

// @symbol _ZN16daObjFl_Seesaw_c16CleanupResourcesEv
s32 daObjFl_Seesaw_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov022_021145a8.Release();
    data_ov022_021145a0.Release();
    return 1;
}

// @symbol _ZN16daObjFl_Seesaw_c6RenderEv
s32 daObjFl_Seesaw_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* Slot 6. */
// @symbol _ZN16daObjFl_Seesaw_c8BehaviorEv
s32 daObjFl_Seesaw_c::Behavior()
{
    func_020393a4((int *)&mMeshCollider, 0x650000);
    if (DecIfAbove0_Byte(&mSwingCooldown) == 0) {
        mAngleX += mSwingStep;
        if (mAngleX >= 0x400 || mAngleX <= -0x400) {
            mSwingStep = -mSwingStep;
            mSwingCooldown = 0x1e;
        }
    }
    func_ov022_02111d48();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

/* Slot 0. */
// @symbol _ZN16daObjFl_Seesaw_c13InitResourcesEv
s32 daObjFl_Seesaw_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov022_021145a8), 1, -1);
    func_ov022_02111d48();
    UpdateClsnPosAndRot();
    char *kcl = dBgW_Kc::LoadFile(data_ov022_021145a0);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, data_ov064_0211bacc);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    mSwingStep = -0x10;
    return 1;
}

// @symbol daObjFl_Seesaw_c_classInit
extern "C" daObjFl_Seesaw_c *daObjFl_Seesaw_c_classInit()
{
    return new daObjFl_Seesaw_c();
}
