//cpp
// @symbol _ZN14RotatingBridge13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingBridge.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* c, void* file, int a, int b);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void* c);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* c);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* f);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* c, void* kcl, void* m, int fix, short s, void* clps);
extern void func_020393d4(int* p, void* v);
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
}

int RotatingBridge::InitResources()
{
  void* f;
  f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov015_02114a8c);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, f, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov015_02114a84);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      ((char*)this) + 0x124, f, ((char*)this) + 0x2ec, 0x199, mAngleY, &data_ov015_02113654);
  func_020393d4((int*)((char*)&(*(u8 *)&mMeshCollider)), (void*)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  mPauseTimer = 0x3c;
  return 1;
}
