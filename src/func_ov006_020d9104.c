// @symbol func_ov006_020d9104
// @emits dScMgBomroom_c_OnYoshiTryEat_020d9104
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgBomroom_c::OnYoshiTryEat - recovered from vtable slot identity */
void dScMgBomroom_c_OnYoshiTryEat_020d9104(unsigned char* c){
  func_ov006_020d907c(c);
  unsigned char* a=c+0x6200;
  unsigned char* b=c+0x6000;
  *(unsigned short*)(a+0xee)=0;
  *(int*)(b+0x2d0)=0;
  G2x_SetBlendAlpha((void*)0x4000050,1,0x1c,4,3);
  SetBg0Offset(0,0);
}
