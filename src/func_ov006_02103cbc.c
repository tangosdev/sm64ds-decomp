// @symbol func_ov006_02103cbc
// @emits dScMgPachinko2_c_OnYoshiTryEat_02103cbc
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgPachinko2_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void FreeGfxSlotsById(int n);
void dScMgPachinko2_c_OnYoshiTryEat_02103cbc(char *c, int n){
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
