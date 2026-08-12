// @symbol func_ov006_020d9104
// recovered name: dScMgBomroom_c_OnYoshiTryEat_020d9104
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
void func_ov006_020d9104(unsigned char* c){
  func_ov006_020d907c(c);
  unsigned char* a=c+0x6200;
  unsigned char* b=c+0x6000;
  *(unsigned short*)(a+0xee)=0;
  *(int*)(b+0x2d0)=0;
  _ZN3G2x13SetBlendAlphaEPVttttt((void*)0x4000050,1,0x1c,4,3);
  SetBg0Offset(0,0);
}
