//cpp
// @symbol _ZN9MovingBar13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MovingBar.h"
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
extern void func_020393d4(int* p, int v);
extern int func_01ffb0a4(void*);
extern int data_ov015_02114534[];
extern int _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
}

int MovingBar::InitResources()
{
  int b = (actorID == 0x35) ? 1 : 0;
  if(b) mVariant = 1; else mVariant = 0;
  int j0 = mVariant * 0xc;
  int m = _ZN5Model8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov015_02114534 + j0));
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)&mModel, m, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  int j = mVariant * 0xc;
  int k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov015_02114538 + j));
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)&mMeshCollider, k, (char*)&mClsnMat, 0x1000, mAngleY, *(void**)((char*)data_ov015_0211453c + j));
  func_020393d4((int*)&mMeshCollider, (int)&_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  int tmp[3];
  tmp[0] = 0x1000; tmp[1] = 0; tmp[2] = 0;
  func_01ffb0a4((char*)&mMeshCollider);
  func_01ffb07c((char*)&mMeshCollider, tmp);
  func_020396d0((int*)&mMeshCollider, 0xccd);
  mHomePosX = mPosX;
  mHomePosY = mPosY;
  mHomePosZ = mPosZ;
  func_ov015_02111fb8(((char*)this), 5);
  return 1;
}
