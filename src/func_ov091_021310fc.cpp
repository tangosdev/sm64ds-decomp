//cpp
// @symbol func_ov091_021310fc
// recovered name: RotatingUpDownPlatformUtm_OnHitByMegaChar
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Platform.h"
/* recovered: renamed to Class_Method */
/* daObjRotateUpdownLift_c::OnHitByMegaChar - recovered from vtable slot identity */
extern "C" {
extern void _ZN6Player16IncMegaKillCountEv(void*);
extern void func_02012694(int a, void* b);
void func_ov091_021310fc(char* self, void* p){
  unsigned short h = *(unsigned short*)(self+0xc);
  int eq = (h == 0x1e);
  if(eq) return;
  _ZN6Player16IncMegaKillCountEv(p);
  func_02012694(0x1e, self+0x74);
  _ZN8Platform14KillByMegaCharER6Player(self, p);
  *(short*)(self+0x8e)=*(short*)(self+0x94);
}
}
