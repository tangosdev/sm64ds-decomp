//cpp
// @symbol _ZN11VolcanoRing13InitResourcesEv
#include "VolcanoRing.h"

/* VolcanoRing::InitResources -- vtable slot 0, ov022 0x0211149c.
 *
 * Real member function on real fields, same idiom as
 * src/_ZN8SignPost13InitResourcesEv.cpp (#1554): mModel/mMeshCollider/
 * mClsnMat/mAngleY/mAngleZ/mPrevAngleZ are all dBgActor_c's/dActor_c's own
 * named fields (include/dBgActor_c.h, include/dActor_c.h). ModelBase::SetFile
 * is a real method call. Model::LoadFile, dBgW_Kc::LoadFile and
 * dBgW_KcMbg::SetFile stay extern "C" under their exact ROM symbols
 * -- dBgW_KcMbg::SetFile takes Fix12<int> BY VALUE, which cannot be
 * declared callable here without changing the argument-passing ABI
 * (mwccarm-codegen.md 6az), same reasoning as SignPost's.
 *
 * data_ov022_02113cc8 is this class's own file table: model, kcl, clps. Kept
 * as the pre-migration local SF3 shape (three raw pointers) -- it is this
 * class's private data, not a shared type.
 *
 * func_020393d4/func_020393c4 (SetCollisionCallback-shaped) and
 * func_ov022_02111564 (this class's own, still unmigrated) stay under their
 * existing names, called exactly as the pre-migration recovery called
 * them. */
typedef struct { void *a, *b, *c; } SF3;
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_020393d4(void *p, void *v);
extern void func_020393c4(void *p, void *v);
extern SF3 data_ov022_02113cc8;
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_[];
extern void func_ov022_02111564(void *);
}

int VolcanoRing::InitResources()
{
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov022_02113cc8.a);
    mModel.SetFile((BMD_File *)f, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov022_02113cc8.b);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x199, mAngleY, data_ov022_02113cc8.c);
    func_020393d4(&mMeshCollider, _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(&mMeshCollider, (void *)func_ov022_02111564);
    mPrevAngleZ = -0x100;
    if (mAngleZ != 0) mPrevAngleZ = mAngleZ;
    return 1;
}
