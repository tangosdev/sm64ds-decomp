// @symbol func_ov006_020eda48
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgHanachan_c.h"
// @emits dScMgHanachan_c_OnYoshiTryEat_020eda48
/* recovered: renamed to Class_Method */
/* dScMgHanachan_c::OnYoshiTryEat - recovered from vtable slot identity */
extern char* func_020beb68;

void dScMgHanachan_c_OnYoshiTryEat_020eda48(char* c, int state)
{
    struct dScMgHanachan_c *self = (struct dScMgHanachan_c *)(void *)c;
    if (state == 1) {
        unsigned int v = self->unk_0bc + 1;
        self->unk_0bc = v;
        if (self->unk_0bc > 0x270e) self->unk_0bc = 0x270e;
    } else if (state == 0x12) {
        self->unk_0bc = 0;
        if (self->unk_0bc > 0x270e) self->unk_0bc = 0x270e;
        if (func_020beb68 != 0) *(int*)(func_020beb68 + 0xb4) = 0;
        func_ov004_020adb1c(self->unk_0b4);
        self->unk_4670 = 0x14;
    } else {
        int v = self->unk_0bc;
        if (func_020beb68 != 0) *(int*)(func_020beb68 + 0xb4) = v;
        self->unk_4670 = 0x14;
    }
    func_ov006_020ea8e0();
    func_ov006_020ea658();
    func_ov006_020ed8a4(c);
}
