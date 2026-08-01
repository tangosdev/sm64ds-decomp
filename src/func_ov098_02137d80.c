// @symbol func_ov098_02137d80
// recovered name: ArrowSignRight_OnHitByMegaChar
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Platform.h"
/* recovered: renamed to Class_Method */
/* daObjYajirusi_c::OnHitByMegaChar - recovered from vtable slot identity */
extern int func_02012694();
extern void _ZN6Player16IncMegaKillCountEv(void*);
void func_ov098_02137d80(void* a, void* b){
  _ZN6Player16IncMegaKillCountEv(b);
  func_02012694(0x1e,(char*)a+0x74);
  _ZN8Platform14KillByMegaCharER6Player(a,b);
}
