//cpp
// @symbol _ZN21ArmedRotatingPlatform13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ArmedRotatingPlatform.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern int func_020393d4(void*, void*);
}
extern void* data_ov036_02113d78[];
extern "C" {
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
}

int ArmedRotatingPlatform::InitResources()
{
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov036_02113d78[0]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, mdl, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
  kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov036_02113d78[1]);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, data_ov036_02113d78[2]);
  func_020393d4(&mMeshCollider, &_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  mAngVelY = -0x80;
  if (mAngleZ != 0) mAngVelY = mAngleZ;
  return 1;
}
