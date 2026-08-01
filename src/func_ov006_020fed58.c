// @symbol func_ov006_020fed58
// recovered name: dScMgPachinko_c_OnYoshiTryEat_020fed58
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgPachinko_c::OnYoshiTryEat - recovered from vtable slot identity */
void func_ov006_020fed58(char *c, int n){
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
