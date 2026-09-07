//cpp
// @symbol _ZN5Stump13InitResourcesEv
#include "Stump.h"

/* Stump::InitResources -- vtable slot 0, ov091 0x021338ac.
 *
 * Real member function, same idiom as src/_ZN8SignPost13InitResourcesEv.cpp
 * and src/_ZN11VolcanoRing13InitResourcesEv.cpp (#1554): mModel/mMeshCollider/
 * mClsnMat/mAngleY are dBgActor_c's/dActor_c's own named fields. mState is
 * this class's own (include/Stump.h). Model::LoadFile, dBgW_Kc::LoadFile
 * and dBgW_KcMbg::SetFile stay extern "C" under their exact ROM
 * symbols -- the last takes Fix12<int> BY VALUE, which the bytes refuse to
 * accept as a real parameter (mwccarm-codegen.md 6az). */
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern int data_ov091_02135654[];
extern int data_ov091_0213564c[];
extern int data_ov002_0210d874[];
}

int Stump::InitResources()
{
    void *m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov091_02135654);
    mModel.SetFile((BMD_File *)m, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov091_0213564c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x199, mAngleY, data_ov002_0210d874);
    mState = 3;
    return 1;
}
