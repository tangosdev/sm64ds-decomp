//cpp
// @symbol _ZN14dScMgMemory2_c13OnYoshiTryEatEi
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory2_c.h"

extern "C" {
// recovered name: dScMgMemory2_c_OnYoshiTryEat_020f7394
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
extern void func_ov004_020b66d4(void *o);
/* data_ov004_020beb68 is declared `void *` by dScMgBase_c.h; cast at the use site. */

void dScMgMemory2_c::OnYoshiTryEat(int /* arg */)
{
    char *c = (char *)this;

    char *o;
    int v;
    ResetGame();
    mState = 0;
    o = (char *)data_ov004_020beb68;
    v = 0;
    if (o != 0) v = *(int *)(o + 0xa8);
    if (v >= 5) v = 5;
    func_ov004_020b66d4(o);
    data_ov004_020bc7d4 = 1;
    mMaxMisses = (unsigned char)v;
}
}
