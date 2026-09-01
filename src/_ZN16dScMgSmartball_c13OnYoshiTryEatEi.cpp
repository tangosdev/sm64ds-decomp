//cpp
#include "types.h"
#include "dScMgSmartball_c.h"
// @symbol _ZN16dScMgSmartball_c13OnYoshiTryEatEi
// recovered name: dScMgSmartball_c_OnYoshiTryEat_02118a8c
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
extern "C" {
void func_ov006_02115b0c(void);
void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void* p, u16 a, u16 b, u16 c, u16 d);
void dScMgSmartball_c::OnYoshiTryEat(int /* arg */)
{
  func_ov006_02115b0c();
  _ZN3G2x13SetBlendAlphaEPVttttj((volatile void*)0x4000050, 0, 0x18, 4, 0xa);
  _ZN3G2x13SetBlendAlphaEPVttttj((volatile void*)0x4001050, 0, 0x18, 4, 0xa);
}
}
