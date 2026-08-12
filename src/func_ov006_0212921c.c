// @symbol func_ov006_0212921c
// recovered name: dScMgSnowball_c_OnYoshiTryEat_0212921c
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern void func_ov006_02126a98(void* a);
void func_ov006_0212921c(void* c, int i){
  func_ov004_020adb1c(0);
  func_ov006_021279b0(c);
  if(i != 0x13) return;
  func_ov006_02126ee4(c);
  func_ov006_02126a98(c);
}
