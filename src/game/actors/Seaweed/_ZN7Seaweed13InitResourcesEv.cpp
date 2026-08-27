//cpp
// @symbol _ZN7Seaweed13InitResourcesEv
#include "Seaweed.h"
// recovered name: daObjWakame_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjWakame_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char*, void*, int, int);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char*, void*, int, int, unsigned int);
s32 Seaweed::InitResources() {
    char* c = (char*)this;
  void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210e0dc);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, m, 1, -1);
  void* a = _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e0d4);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, a, 0, 0x1000, 0);
  func_ov002_020bc488(c);
  return 1;
}
}
