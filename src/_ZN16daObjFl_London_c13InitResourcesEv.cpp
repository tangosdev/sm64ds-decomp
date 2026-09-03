//cpp
// @symbol _ZN16daObjFl_London_c13InitResourcesEv
#include "daObjFl_London_c.h"

/* daObjFl_London_c::InitResources -- vtable slot 0, ov022 0x02111bdc.
 *
 * Real member function on real fields, same idiom as
 * src/_ZN11VolcanoRing13InitResourcesEv.cpp (#1583). func_ov022_02111a1c is
 * this class's own, still-unmigrated helper (out of this task's scope),
 * called under its existing name. mCooldown/mFlag are this class's own
 * fields (include/daObjFl_London_c.h). */
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_ov022_02111a1c(void *self);
extern void func_020393d4(void *p, void *v);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
extern void *data_ov022_02114580;
extern void *data_ov022_02114578;
extern void *data_ov064_0211bb2c;
}

int daObjFl_London_c::InitResources()
{
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov022_02114580);
    mModel.SetFile((BMD_File *)f, 1, -1);
    func_ov022_02111a1c(this);
    UpdateClsnPosAndRot();
    void *k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov022_02114578);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, &data_ov064_0211bb2c);
    func_020393d4(&mMeshCollider, (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    mFlag = 0;
    mCooldown = 0xf;
    return 1;
}
