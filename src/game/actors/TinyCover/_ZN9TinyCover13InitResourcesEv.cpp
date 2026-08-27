//cpp
// @symbol _ZN9TinyCover13InitResourcesEv
#include "TinyCover.h"
// recovered name: daObjTtFuta_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjTtFuta_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern int _ZN5Event6GetBitEj(unsigned int);
extern int data_ov033_02111bfc[];
s32 TinyCover::InitResources() {
    char * c = (char *)this;
  void *f = _ZN5Model8LoadFileER13SharedFilePtr((void*)data_ov033_021124c8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  void *k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr((void*)data_ov033_021124c0);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, k, c+0x2ec, 0x199, *(short*)(c+0x8e), data_ov033_02111bfc);
  return _ZN5Event6GetBitEj(0xe) == 0;
}
}
