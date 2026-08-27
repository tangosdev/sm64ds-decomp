//cpp
// @symbol _ZN21ClockPaintingPendulum13InitResourcesEv
// recovered name: ClockPaintingPendulum::InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, vtable slot 0 */
/* ClockPaintingPendulum::InitResources -- vtable slot 0, ov013 0x0211133c.
 * Same idiom as src/_ZN4Door13InitResourcesEv.c: declared as an override in
 * include/ClockPaintingPendulum.h, defined here under the mangled symbol,
 * not as a real ClockPaintingPendulum:: method. */
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int __sinit_ov045_02112280[];
extern void func_ov013_02111238(char *t);
int _ZN21ClockPaintingPendulum13InitResourcesEv(char *c){
  void *f = _ZN5Model8LoadFileER13SharedFilePtr((void*)__sinit_ov045_02112280);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  func_ov013_02111238(c);
  *(short*)(c+0x124)=0x100;
  return 1;
}
}
