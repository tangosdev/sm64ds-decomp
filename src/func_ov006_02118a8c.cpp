//cpp
#include "types.h"
// @symbol func_ov006_02118a8c
// recovered name: dScMgSmartball_c_OnYoshiTryEat_02118a8c
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern "C" {
void func_ov006_02115b0c(void);
void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void* p, u16 a, u16 b, u16 c, u16 d);
void func_ov006_02118a8c(void){
  func_ov006_02115b0c();
  _ZN3G2x13SetBlendAlphaEPVttttt((volatile void*)0x4000050, 0, 0x18, 4, 0xa);
  _ZN3G2x13SetBlendAlphaEPVttttt((volatile void*)0x4001050, 0, 0x18, 4, 0xa);
}
}
