//cpp
// @symbol func_ov013_0211133c
// recovered name: daObjClockHuriko_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjClockHuriko_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int __sinit_ov045_02112280[];
extern void func_ov013_02111238(char *t);
int func_ov013_0211133c(char *c){
  void *f = _ZN5Model8LoadFileER13SharedFilePtr((void*)__sinit_ov045_02112280);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  func_ov013_02111238(c);
  *(short*)(c+0x124)=0x100;
  return 1;
}
}
