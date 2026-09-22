//cpp
// @symbol _ZN13BowserShutter13InitResourcesEv
#include "BowserShutter.h"
// recovered name: daObjWlKoopaShutter_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjWlKoopaShutter_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(void *);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, int, int, int);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
extern int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, int, void *, int, int, void *);
extern void _ZN4dBgW6EnableEP8dActor_c(void *, void *);
extern char data_0209caa0[];
}

s32 BowserShutter::InitResources() {
    char * c = (char *)this;
  void *m = (void*)_ZN5Model8LoadFileER13SharedFilePtr(data_ov026_02113ebc);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4,(int)m,1,-1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  {
    void *k = (void*)_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov026_02113eb4);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c+0x124,(int)k,c+0x2ec,0x199,*(short*)(c+0x8e),(void*)data_ov026_02112f74);
  }
  if(*(int*)(data_0209caa0+4) & 0x204) return 0;
  _ZN4dBgW6EnableEP8dActor_c(c+0x124,c);
  return 1;
}
