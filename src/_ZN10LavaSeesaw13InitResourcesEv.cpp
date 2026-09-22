//cpp
// @symbol _ZN10LavaSeesaw13InitResourcesEv
#include "LavaSeesaw.h"

/* LavaSeesaw::InitResources -- vtable slot 0, ov022 0x02111ea0.
 *
 * Real member function on real fields, same idiom as
 * src/_ZN11VolcanoRing13InitResourcesEv.cpp (#1583): mModel/mMeshCollider/
 * mClsnMat/mAngleY are dBgActor_c's/dActor_c's own named fields.
 * ModelBase::SetFile and UpdateClsnPosAndRot are real method calls.
 * Model::LoadFile, dBgW_Kc::LoadFile and dBgW_KcMbg::SetFile
 * stay extern "C" under their exact ROM symbols -- the last takes
 * Fix12<int> BY VALUE, which the bytes refuse as a real parameter
 * (mwccarm-codegen.md 6az). func_ov022_02111d48 and
 * func_020393d4/dBgW::UpdatePosWithTransform are this class's
 * own file-load bookkeeping, still unmigrated (out of this task's scope),
 * called under their existing names -- same as VolcanoRing's. */
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_ov022_02111d48(void *self);
extern void func_020393d4(void *p, void *v);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
extern void *data_ov022_021145a8;
extern void *data_ov022_021145a0;
extern void *data_ov064_0211bacc;
}

int LavaSeesaw::InitResources()
{
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov022_021145a8);
    mModel.SetFile((BMD_File *)f, 1, -1);
    func_ov022_02111d48(this);
    UpdateClsnPosAndRot();
    void *k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov022_021145a0);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, &data_ov064_0211bacc);
    func_020393d4(&mMeshCollider, (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    mSwingStep = -0x10;
    return 1;
}
