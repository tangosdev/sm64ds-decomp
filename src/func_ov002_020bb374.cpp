//cpp
// @symbol func_ov002_020bb374
// recovered name: SignPost_OnHitByMegaChar
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Platform.h"
/* recovered: renamed to Class_Method */
/* daObjTatefuda_c::OnHitByMegaChar - recovered from vtable slot identity */
extern "C" {
extern void _ZN6Player16IncMegaKillCountEv(void*);
extern void func_02012694(int a, void* b);
void func_ov002_020bb374(char* self, void* p){
  _ZN6Player16IncMegaKillCountEv(p);
  func_02012694(0x1d, self+0x74);
  _ZN8Platform14KillByMegaCharER6Player(self, p);
  *(short*)(self+0x8e)=*(short*)(self+0x94);
}
}
