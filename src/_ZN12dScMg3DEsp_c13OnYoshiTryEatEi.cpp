//cpp
// @symbol _ZN12dScMg3DEsp_c13OnYoshiTryEatEi
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dMg3DEspModel_c.h"
#include "dScMg3DEsp_c.h"
// recovered name: dScMg3DEsp_c_OnYoshiTryEat_020e9c20
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
/* data_ov004_020beb68 is declared `void *` by dScMgBase_c.h; cast at the use site
   rather than redeclaring it with a different pointer type. */

void dScMg3DEsp_c::OnYoshiTryEat(int a)
{
    char *c = (char *)this;

    dScMg3DEsp_c *self = (dScMg3DEsp_c *)(void *)c;
    func_ov006_020e984c(c);
    self->unk_553c = 0;
    if (a == 0) {
        int* p = (int*)(((int)c + 0xbc));
        *p += 1;
        if (self->unk_0bc > 0x270e) self->unk_0bc = 0x270e;
    } else if (a == 0x12) {
        if (data_ov004_020beb68 != 0) *(int*)((char*)data_ov004_020beb68 + 0xb4) = 0;
        self->unk_0bc = 0;
        if (self->unk_0bc > 0x270e) self->unk_0bc = 0x270e;
    }
    {
        self->unk_51e4 = 0;
        ((dMg3DEspModel_c*)(c + 0x4fd8))->Reset();
    }
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
}
