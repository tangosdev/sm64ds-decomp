//cpp
// @symbol _ZN10daPgDfdr_c8BehaviorEv
/* daPgDfdr_c::Behavior -- vtable slot 6. Extern "C" free function under the
   mangled name; see src/_ZN7daDgr_c13InitResourcesEv.cpp for why it is not
   converted to a true method body. */
#include "decl_Player.h"
#include "decl_common.h"
extern "C" {
extern void _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void* t, int a, int b);
extern void* _ZN8dActor_c13ClosestPlayerEv(void* c);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void _ZN5dCc_c5ClearEv(void* a);
extern void _ZN5dCc_c6UpdateEv(void* a);
extern void func_ov027_02111994(char* c);

int _ZN10daPgDfdr_c8BehaviorEv(char* c){
  _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0);
  func_ov027_02111cfc(c);
  if(_ZN6Player16IsInsideOfCannonEv(_ZN8dActor_c13ClosestPlayerEv(c))){
    *(int *)(((int)c + 0xb0)) &= ~2;
  } else {
    *(int *)(((int)c + 0xb0)) |= 2;
  }
  _ZN9Animation7AdvanceEv(c+0x370);
  _ZN9Animation7AdvanceEv(c+0x384);
  _ZN5dCc_c5ClearEv(c+0x398);
  _ZN5dCc_c6UpdateEv(c+0x398);
  func_ov027_02111994(c);
  return 1;
}
}
