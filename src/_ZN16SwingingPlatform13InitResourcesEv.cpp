//cpp
// @symbol _ZN16SwingingPlatform13InitResourcesEv
#include "SwingingPlatform.h"

/* SwingingPlatform::InitResources -- vtable slot 0, ov036 0x0211137c.
 *
 * Real member function on real fields, same idiom as
 * src/_ZN11VolcanoRing13InitResourcesEv.cpp (#1583): mModel/mMeshCollider/
 * mClsnMat/mAngleY/mAngleZ are dBgActor_c's/dActor_c's own named fields.
 * ModelBase::SetFile and UpdateClsnPosAndRot are real method calls.
 * Model::LoadFile, dBgW_Kc::LoadFile and dBgW_KcMbg::SetFile
 * stay extern "C" under their exact ROM symbols -- the last takes
 * Fix12<int> BY VALUE, which the bytes refuse as a real parameter
 * (mwccarm-codegen.md 6az). func_ov036_0211123c/020393d4 and the
 * dBgW transform-update pointer are this class's own,
 * still-unmigrated helpers (out of this task's scope), called under their
 * existing names. */
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_ov036_0211123c(void *self);
extern void func_020393d4(void *p, void *v);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
extern void *data_ov036_02114028;
extern void *data_ov036_02114020;
extern void *data_ov036_02112b68;
}

int SwingingPlatform::InitResources()
{
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov036_02114028);
    mModel.SetFile((BMD_File *)f, 1, -1);
    mAngleZ = 0x2000;
    func_ov036_0211123c(this);
    UpdateClsnPosAndRot();
    void *k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov036_02114020);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, &data_ov036_02112b68);
    func_020393d4(&mMeshCollider, (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    return 1;
}
