//cpp
/* Production translation unit for ov022/daObjFl_Seesaw_c.
 * 7 function(s), .text 0x02111cac..0x02111f3c. The Lethal Lava Land seesaw
 * (registry profile FL_SEESAW).
 *
 * NAME: _ZTS16daObjFl_Seesaw_c is "16daObjFl_Seesaw_c" at ov022 0x02113ffc;
 * _ZTI at 0x02113ff0 reads [__si_class_type_info, that string,
 * _ZTI10dBgActor_c]. The vtable's address point is 0x02114034; the word
 * before it is that _ZTI. The tree previously called the class LavaSeesaw
 * (coined).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02111cac), D0
 * (0x02111cf0), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is
 * ROM-ascending. The factory daObjFl_Seesaw_c_classInit (0x02111f3c) is the
 * next function and stays in its own source, src/d_a_obj_fl_seesaw.c.
 */

#include "math/Matrix.h"
#include "daObjFl_Seesaw_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

extern "C" {
void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void func_020393a4(int *p, int v);
void func_020393d4(int *p, int v);
unsigned char DecIfAbove0_Byte(unsigned char *p);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int radius, int yOffset);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thiz, void *f, const Matrix4x3 *m, int fix, short s, void *b);

extern int data_ov022_021145a8[];
extern int data_ov022_021145a0[];
extern CLPS_Block data_ov064_0211bacc;
}

#pragma defer_codegen off

/* Vtable slots 16 (D1) and 17 (D0). D1 is one vtable store, the members in
 * reverse, then ~dBgActor_c. D0 is the deleting destructor: it destroys
 * through this class and its bases, which is why more than one vptr store
 * appears, then frees through an inline operator delete, which is why
 * nothing here mentions a heap. */
// @symbol _ZN16daObjFl_Seesaw_cD1Ev
// @symbol _ZN16daObjFl_Seesaw_cD0Ev
daObjFl_Seesaw_c::~daObjFl_Seesaw_c()
{
}

// @symbol func_ov022_02111d48
extern "C" void func_ov022_02111d48(char *raw)
{
    daObjFl_Seesaw_c *t = (daObjFl_Seesaw_c *)raw;
    Matrix4x3_FromRotationXYZExt(&t->mModel.mat4x3, t->mAngleX, t->mAngleY, t->mAngleZ);
    t->mModel.mat4x3.t.x = t->mPosX >> 3;
    t->mModel.mat4x3.t.y = t->mPosY >> 3;
    t->mModel.mat4x3.t.z = t->mPosZ >> 3;
}

// @symbol _ZN16daObjFl_Seesaw_c16CleanupResourcesEv
s32 daObjFl_Seesaw_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    ((SharedFilePtr *)data_ov022_021145a8)->Release();
    ((SharedFilePtr *)data_ov022_021145a0)->Release();
    return 1;
}

// @symbol _ZN16daObjFl_Seesaw_c6RenderEv
s32 daObjFl_Seesaw_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* Slot 6. mSwingStep/mSwingCooldown are this class's own fields
 * (daObjFl_Seesaw_c.h). dBgActor_c::IsClsnInRange stays extern "C" under its
 * ROM symbol: it takes Fix12<int> by value, which the bytes refuse as a real
 * parameter (dBgActor_c.h's own note). */
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
    func_ov022_02111d48((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

/* Slot 0. ModelBase::SetFile and UpdateClsnPosAndRot are real method calls.
 * dBgW_KcMbg::SetFile stays extern "C" under its exact ROM symbol: it takes
 * Fix12<int> BY VALUE, which the bytes refuse as a real parameter
 * (mwccarm-codegen.md 6az). */
// @symbol _ZN16daObjFl_Seesaw_c13InitResourcesEv
s32 daObjFl_Seesaw_c::InitResources()
{
    void *f = Model::LoadFile(*(SharedFilePtr *)data_ov022_021145a8);
    mModel.SetFile((BMD_File *)f, 1, -1);
    func_ov022_02111d48((char *)this);
    UpdateClsnPosAndRot();
    void *k = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov022_021145a0);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, &data_ov064_0211bacc);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    mSwingStep = -0x10;
    return 1;
}
