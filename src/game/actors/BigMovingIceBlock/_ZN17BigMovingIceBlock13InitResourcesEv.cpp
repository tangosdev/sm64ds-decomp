//cpp
// @symbol _ZN17BigMovingIceBlock13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "BigMovingIceBlock.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(int* p, void* v);
extern void _ZN7PathPtr6FromIDEj(void* self, unsigned int id);
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
struct SFP { void* a; void* b; void* c; };
extern struct SFP data_ov056_02113314;
}

int BigMovingIceBlock::InitResources()
{
  void* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov056_02113314.a);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, f, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  void* f2 = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov056_02113314.b);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char*)this)+0x124, f2, ((char*)this)+0x2ec, 0x199, mAngleY, data_ov056_02113314.c);
  func_020393d4((int*)((char*)&mMeshCollider), &_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  _ZN7PathPtr6FromIDEj(((char*)this)+0x320, param1&0xff);
  mPathDir = 1;
  mHorzSpeed = 0xa000;
  return 1;
}
