//cpp
// @symbol _ZN16SpinningPlatform13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SpinningPlatform.h"
extern "C" void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
struct BMD_File; struct KCL_File; struct dActor_c; struct Vector3; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr;
/* ModelBase is the real class now, through this actor's header. */
struct dBgActor_c { void UpdateClsnPosAndRot(); };
/* Declared by final name, not as a member: the ROM's SetFile takes Fix12<int> where
   this call passes 0x1000, and Fix12<int> is an aggregate with no converting
   constructor from int. Declared `int fix` it mangles ...RK9Matrix4x3isR10CLPS_Block,
   which nothing defines. */
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);
extern "C" void func_020393d4(void *p, void *v);
struct dBgCh_Gnd {
    int pad[0x11];
    int result;       // offset 0x44
    int pad2[3];      // pad to 0x54 total
    dBgCh_Gnd();
    void SetObjAndPos(const Vector3 &v, dActor_c *a);
    int DetectClsn();
    ~dBgCh_Gnd();
};
extern "C" CLPS_Block data_ov035_02112238;

struct V3 { int x, y, z; };

int SpinningPlatform::InitResources()
{
    void *mf = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov035_02112cb0);
    ((ModelBase*)((char *)&mModel))->SetFile((BMD_File*)mf, 1, -1);
    ((ShadowModel*)((char *)&mShadowModel))->InitCuboid();
    func_ov035_021118a8(((char *)this));
    ((dBgActor_c*)((char *)this))->UpdateClsnPosAndRot();
    void *kf = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov035_02112cb8);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (char *)&mMeshCollider, (KCL_File*)kf,
        mClsnMat, 0x1000, mAngleY, data_ov035_02112238);
    func_020393d4(((char *)this) + 0x124, (void*)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020396c0(((char *)this) + 0x124, 0);
    V3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = v.y - 0xa000;
    dBgCh_Gnd rg;
    rg.SetObjAndPos(*(Vector3*)&v, (dActor_c*)0);
    mFloorPosY = v.y;
    if (rg.DetectClsn() != 0)
        mFloorPosY = rg.result;
    return 1;
}
