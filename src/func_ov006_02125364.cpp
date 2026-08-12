//cpp
// @symbol func_ov006_02125364
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgBSC_c.h"
// recovered name: dScMgBSC_c_OnYoshiTryEat_02125364
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern "C" void func_ov004_020b66d4(void);
extern "C" void func_ov006_02125364(char* c, int mode){
    dScMgBSC_c *self = (dScMgBSC_c *)(void *)c;
  if(mode != 4 && mode != 5 && mode == 3){
    self->unk_0b4 = func_ov004_020ad878();
    func_ov004_020adb1c(self->unk_0b4);
  }
  func_ov004_020b66d4();
  func_020bc7d4 = 1;
  *(int*)(c+0x5000+0x1b8) = 0;
}
