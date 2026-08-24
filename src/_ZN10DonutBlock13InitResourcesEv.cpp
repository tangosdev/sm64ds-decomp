//cpp
// @symbol _ZN10DonutBlock13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "DonutBlock.h"
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void*);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
void func_020393d4(void* p, void* v);
void func_020393c4(void* p, void* v);
int _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void*,void*,int,int,int,int);
extern void _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
}

int DonutBlock::InitResources()
{
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov036_0211408c);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, m, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  int k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov036_02114084);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, data_ov036_02112b48);
  func_020393d4(&mMeshCollider, (void*)_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  func_020393c4(&mMeshCollider, (void*)func_ov036_02111cc4);
  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x320, ((char*)this), 0x32000, 0x64000, 0, 0);
  mTerminalVelocity = -0x1e000;
  mVertAccel = ~0x198;
  mHomePosX = mPosX;
  mHomePosY = mPosY;
  mHomePosZ = mPosZ;
  return 1;
}
