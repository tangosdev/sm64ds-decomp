//cpp
// @symbol _ZN8Squasher13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_RotatingPlatformWdw.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Squasher.h"
#include "dBgW.h"
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void* f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* m, int bmd, int a, int b);
void _ZN11ShadowModel10InitCuboidEv(void* s);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* c);
int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* mc, int kcl, struct Matrix4x3* mtx, int scale, short s, void* clps);
void func_020393d4(int* p, int v);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
}

int Squasher::InitResources()
{
  int bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov023_02112088);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, bmd, 1, -1);
  _ZN11ShadowModel10InitCuboidEv((char*)&mShadowModel);
  func_ov023_02111308(((char*)this));
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  int kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&_ZN19RotatingPlatformWdwD1Ev);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, kcl, (struct Matrix4x3*)((char*)&(*(u8 *)&mClsnMat)), 0x1000, (*(u16 *)&mAngleY), &data_ov064_0211ba4c);
  func_020393d4((int*)((char*)&mMeshCollider), (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  ((dBgW *)(&mMeshCollider))->Enable((dActor_c *)(((char*)this)));
  mAngVelX = 0;
  mStateTimer = 0;
  mState = 0;
  return 1;
}
