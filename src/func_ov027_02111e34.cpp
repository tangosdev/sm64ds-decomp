//cpp
// @symbol func_ov027_02111e34
// @emits daPgDfdr_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Player.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daPgDfdr_c::Behavior - recovered from vtable slot identity */
extern "C" {
extern void _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void* t, int a, int b);
extern void* _ZN5Actor13ClosestPlayerEv(void* c);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void _ZN12CylinderClsn5ClearEv(void* a);
extern void _ZN12CylinderClsn6UpdateEv(void* a);
extern void func_ov027_02111994(char* c);

int daPgDfdr_c_Behavior(char* c){
  _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0);
  func_ov027_02111cfc(c);
  if(_ZN6Player16IsInsideOfCannonEv(_ZN5Actor13ClosestPlayerEv(c))){
    *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~2;
  } else {
    *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) |= 2;
  }
  _ZN9Animation7AdvanceEv(c+0x370);
  _ZN9Animation7AdvanceEv(c+0x384);
  _ZN12CylinderClsn5ClearEv(c+0x398);
  _ZN12CylinderClsn6UpdateEv(c+0x398);
  func_ov027_02111994(c);
  return 1;
}
}
