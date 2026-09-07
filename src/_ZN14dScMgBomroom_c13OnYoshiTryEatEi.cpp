//cpp
// @symbol _ZN14dScMgBomroom_c13OnYoshiTryEatEi
// recovered name: dScMgBomroom_c_OnYoshiTryEat_020d9104
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgBomroom_c.h"

/* decl_common.h does not carry this one, and as a .c file this body simply
   called it undeclared.  Same spelling as src/_ZN16dScMgSmartball_c13OnYoshiTryEatEi.cpp. */
extern "C" {
void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, u16 a, u16 b, u16 c, u16 d);
}
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
void dScMgBomroom_c::OnYoshiTryEat(int /* arg */)
{
    unsigned char *c = (unsigned char *)this;

  func_ov006_020d907c(c);
  unsigned char* a=c+0x6200;
  unsigned char* b=c+0x6000;
  *(unsigned short*)(a+0xee)=0;
  *(int*)(b+0x2d0)=0;
  _ZN3G2x13SetBlendAlphaEPVttttj((volatile void*)0x4000050,1,0x1c,4,3);
  SetBg0Offset(0,0);
}
