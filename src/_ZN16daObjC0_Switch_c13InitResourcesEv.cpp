//cpp
// @symbol _ZN16daObjC0_Switch_c13InitResourcesEv
#include "daObjC0_Switch_c.h"

/* daObjC0_Switch_c::InitResources -- vtable slot 0, ov012 0x02111370.
 *
 * Real member function on real fields, same idiom as
 * src/_ZN11VolcanoRing13InitResourcesEv.cpp (#1583): mModel/mMeshCollider/
 * mClsnMat/mAngleY are dBgActor_c's/dActor_c's own named fields
 * (include/dBgActor_c.h, include/dActor_c.h). ModelBase::SetFile,
 * UpdateModelPosAndRotY and UpdateClsnPosAndRot are real method calls.
 * Model::LoadFile, dBgW_Kc::LoadFile and dBgW_KcMbg::SetFile
 * stay extern "C" under their exact ROM symbols -- dBgW_KcMbg::SetFile
 * takes Fix12<int> BY VALUE, which cannot be declared callable here without
 * changing the argument-passing ABI (mwccarm-codegen.md 6az).
 *
 * data_0209caa0 is the same untyped shared-flags global other TUs reference
 * raw (e.g. src/func_ov002_020d4748.cpp); mPosY -= 0x64000 and mPressed = 1
 * only run when data_0209caa0[2] & 0x80000 is set -- some other pillar in the
 * group already reported pressed (see OnGroundPounded), so this one starts
 * sunk and pre-flagged. */
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern int data_0209caa0[];
extern int data_ov012_021124a8[];
extern int data_ov012_021124a0[];
extern int data_ov012_02111cd0[];
}

int daObjC0_Switch_c::InitResources()
{
    void *mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov012_021124a8);
    mModel.SetFile((BMD_File *)mdl, 1, -1);
    if (data_0209caa0[2] & 0x80000) {
        mPosY -= 0x64000;
        mPressed = 1;
    }
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov012_021124a0);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, data_ov012_02111cd0);
    return 1;
}
