//cpp
// @symbol _ZN13dScMgMemory_c13OnYoshiTryEatEi
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory_c.h"

extern "C" {
// recovered name: dScMgMemory_c_OnYoshiTryEat_020f52c4
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
extern void func_ov004_020b66d4(void *o);
extern void _ZN13dScMgMemory_c9ResetGameEv(void *c);
/* data_ov004_020beb68 is declared `void *` by dScMgBase_c.h; cast at the use site. */

void dScMgMemory_c::OnYoshiTryEat(int /* arg */)
{
    char *c = (char *)this;

    struct dScMgMemory_c *self = (struct dScMgMemory_c *)(void *)c;
    char *o;
    int v;
    _ZN13dScMgMemory_c9ResetGameEv(c);
    self->unk_5314 = 0;
    o = (char *)data_ov004_020beb68;
    v = 0;
    if (o != 0) v = *(int *)(o + 0xa8);
    if (v >= 3) v = 3;
    func_ov004_020b66d4(o);
    data_ov004_020bc7d4 = 1;
    self->unk_533b = (unsigned char)v;
}
}
