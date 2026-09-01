//cpp
// @symbol _ZN16dScMgPachinko2_c13OnYoshiTryEatEi
// recovered name: dScMgPachinko2_c_OnYoshiTryEat_02103cbc
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgPachinko2_c.h"
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
extern "C" {
extern void FreeGfxSlotsById(int n);
}
void dScMgPachinko2_c::OnYoshiTryEat(int n)
{
    char *c = (char *)this;

    *(int*)(c+0x5000+0x660) = 0;
    if(n == 0x10){
        *(int*)(c+0xbc) = *(int*)(c+0xbc) + 1;
        if(*(unsigned int*)(c+0xbc) > 0x270e) *(int*)(c+0xbc) = 0x270e;
    } else {
        *(int*)(c+0xbc) = 0;
        if(*(unsigned int*)(c+0xbc) > 0x270e) *(int*)(c+0xbc) = 0x270e;
    }
    FreeGfxSlotsById(0x1d);
    func_ov006_02103bfc(c);
}
