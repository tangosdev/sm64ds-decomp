//cpp
// @symbol _ZN15dScMgSnowball_c13OnYoshiTryEatEi
// recovered name: dScMgSnowball_c_OnYoshiTryEat_0212921c
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgSnowball_c.h"
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
extern "C" {
extern void func_ov006_02126a98(void* a);
}
void dScMgSnowball_c::OnYoshiTryEat(int i)
{
    void *c = (void *)this;

  func_ov004_020adb1c(0);
  func_ov006_021279b0(c);
  if(i != 0x13) return;
  func_ov006_02126ee4(c);
  func_ov006_02126a98(c);
}
