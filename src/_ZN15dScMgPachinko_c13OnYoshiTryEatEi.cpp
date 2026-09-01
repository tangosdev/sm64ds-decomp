//cpp
// @symbol _ZN15dScMgPachinko_c13OnYoshiTryEatEi
// recovered name: dScMgPachinko_c_OnYoshiTryEat_020fed58
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgPachinko_c.h"
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
void dScMgPachinko_c::OnYoshiTryEat(int n)
{
    char *c = (char *)this;

    *(int*)(c+0x5000+0xc10) = 0;
    if(n == 9){
        *(int*)(c+0xbc) = *(int*)(c+0xbc) + 1;
        if(*(unsigned int*)(c+0xbc) > 0x270e) *(int*)(c+0xbc) = 0x270e;
    } else {
        *(int*)(c+0xbc) = 0;
        if(*(unsigned int*)(c+0xbc) > 0x270e) *(int*)(c+0xbc) = 0x270e;
    }
    func_ov006_020fadfc(c);
    func_ov006_020fad90(c);
}
