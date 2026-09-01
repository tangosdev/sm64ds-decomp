//cpp
// @symbol _ZN12dScMgAmida_c13OnYoshiTryEatEi
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgAmida_c.h"
// recovered name: dScMgAmida_c_OnYoshiTryEat_020d52f0
/* dScMgAmida_c::OnYoshiTryEat - recovered from vtable slot identity. Own new
   slot 18, left as a raw helper (see include/dScMgAmida_c.h's class banner
   -- same precedent as every other dScMgBase_c leaf's slot 18). 0xb4/0xbc
   are dScMgBase_c's own inherited fields (0xbc falls inside that class's
   pad_0bc, not a named field there either), reached via a raw char* offset
   instead of the class header, same precedent dScMgPachinko_c's own slot 18
   helper (src/func_ov006_020fed58.c) already uses for the same offset. */
void dScMgAmida_c::OnYoshiTryEat(int arg)
{
    char *c = (char *)this;

    if (arg == 0) {
        int *p;
        if (*(unsigned int *)(c + 0xbc) >= 0x7cf)
            goto final;
        p = (int *)(c + 0xbc);
        *p = *p + 1;
        if (*(unsigned int *)(c + 0xbc) > 0x270e)
            *(int *)(c + 0xbc) = 0x270e;
    } else {
        *(int *)((c + 0x5000) + 0x3e8) = 0;
        *(int *)(c + 0xbc) = 0;
        if (*(unsigned int *)(c + 0xbc) > 0x270e)
            *(int *)(c + 0xbc) = 0x270e;
        func_ov004_020adb1c(*(int *)((c + 0x5000) + 0x3e8));
        *(int *)(c + 0xb4) = *(int *)((c + 0x5000) + 0x3e8);
    }
final:
    func_ov006_020d3ba0(c);
}
