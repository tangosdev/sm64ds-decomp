//cpp
// @symbol _ZN16SpinningPlatform13InitResourcesEv
#include "SpinningPlatform.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"

extern "C" void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
/* Declared by final name, not as a member: the ROM's SetFile takes Fix12<int> where
   this call passes 0x1000, and Fix12<int> is an aggregate with no converting
   constructor from int. Declared `int fix` it mangles ...RK9Matrix4x3isR10CLPS_Block,
   which nothing defines. */
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);
extern "C" void func_020393d4(void *p, void *v);
extern "C" void func_020396c0(void *p, int value);
extern "C" CLPS_Block data_ov035_02112238;
extern SharedFilePtr data_ov035_02112cb0;
extern SharedFilePtr data_ov035_02112cb8;

struct V3 { int x, y, z; };

int SpinningPlatform::InitResources()
{
    void *mf = Model::LoadFile(data_ov035_02112cb0);
    mModel.SetFile((BMD_File *)mf, 1, -1);
    mShadowModel.InitCuboid();
    UpdateModel();
    UpdateClsnPosAndRot();
    KCL_File *kf = (KCL_File *)dBgW_Kc::LoadFile(data_ov035_02112cb8);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kf,
        mClsnMat, 0x1000, mAngleY, data_ov035_02112238);
    func_020393d4(&mMeshCollider,
                  (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020396c0(&mMeshCollider, 0);
    V3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = v.y - 0xa000;
    dBgCh_Gnd rg;
    rg.SetObjAndPos(*(Vector3 *)&v, (dActor_c *)0);
    mFloorPosY = v.y;
    if (rg.DetectClsn() != 0)
        mFloorPosY = rg.clsnY;
    return 1;
}
