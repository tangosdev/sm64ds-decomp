// @symbol func_ov006_02103cbc
// recovered name: dScMgPachinko2_c_OnYoshiTryEat_02103cbc
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern void FreeGfxSlotsById(int n);
void func_ov006_02103cbc(char *c, int n){
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
