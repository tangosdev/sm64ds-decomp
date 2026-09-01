//cpp
// @symbol _ZN15dScMgHanachan_c13OnYoshiTryEatEi
/* dScMgHanachan_c's own vtable slot 18 -- stays a raw extern "C" helper
   function rather than a migrated class method, same precedent as every
   other dScMgBase_c leaf's slot 18 (see include/dScMgHanachan_c.h). Takes a
   raw char*, not `this`, so 0xbc/0xb4 (both inherited from dScMgBase_c --
   0xbc is inside its own pad_0bc, not a named field there either) and
   0x4670 (this class's own field) all stay raw offsets here rather than
   named member access, same precedent dScMgPachinko2_c.h documents for
   0xbc. */
#include "decl_common.h"
#include "dScMgHanachan_c.h"
extern "C" {
extern char* data_ov004_020beb68;
}

void dScMgHanachan_c::OnYoshiTryEat(int state)
{
    char *c = (char *)this;

    if (state == 1) {
        unsigned int v = *(unsigned int*)(c + 0xbc) + 1;
        *(unsigned int*)(c + 0xbc) = v;
        if (*(unsigned int*)(c + 0xbc) > 0x270e) *(unsigned int*)(c + 0xbc) = 0x270e;
    } else if (state == 0x12) {
        *(unsigned int*)(c + 0xbc) = 0;
        if (*(unsigned int*)(c + 0xbc) > 0x270e) *(unsigned int*)(c + 0xbc) = 0x270e;
        if (data_ov004_020beb68 != 0) *(int*)(data_ov004_020beb68 + 0xb4) = 0;
        func_ov004_020adb1c(*(int*)(c + 0xb4));
        *(int*)(c + 0x4670) = 0x14;
    } else {
        int v = *(unsigned int*)(c + 0xbc);
        if (data_ov004_020beb68 != 0) *(int*)(data_ov004_020beb68 + 0xb4) = v;
        *(int*)(c + 0x4670) = 0x14;
    }
    func_ov006_020ea8e0();
    func_ov006_020ea658();
    func_ov006_020ed8a4(c);
}
