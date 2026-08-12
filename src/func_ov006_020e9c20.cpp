//cpp
// @symbol func_ov006_020e9c20
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMg3DEsp_c.h"
// recovered name: dScMg3DEsp_c_OnYoshiTryEat_020e9c20
/* recovered: renamed to Class_Method */
extern char* func_020beb68;

extern "C" void func_ov006_020e9c20(char* c, int a)
{
    dScMg3DEsp_c *self = (dScMg3DEsp_c *)(void *)c;
    func_ov006_020e984c(c);
    self->unk_553c = 0;
    if (a == 0) {
        int* p = (int*)(((int)c + 0xbc));
        *p += 1;
        if (self->unk_0bc > 0x270e) self->unk_0bc = 0x270e;
    } else if (a == 0x12) {
        if (func_020beb68 != 0) *(int*)(func_020beb68 + 0xb4) = 0;
        self->unk_0bc = 0;
        if (self->unk_0bc > 0x270e) self->unk_0bc = 0x270e;
    }
    {
        char* dest = (char*)(c + 0x4fd8);
        self->unk_51e4 = 0;
        func_ov006_020e7fb0(dest);
    }
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
}
