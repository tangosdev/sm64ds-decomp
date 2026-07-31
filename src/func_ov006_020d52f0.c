// @symbol func_ov006_020d52f0
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgAmida_c.h"
// @emits dScMgAmida_c_OnYoshiTryEat_020d52f0
/* recovered: renamed to Class_Method */
/* dScMgAmida_c::OnYoshiTryEat - recovered from vtable slot identity */
void dScMgAmida_c_OnYoshiTryEat_020d52f0(char *c, int arg)
{
    struct dScMgAmida_c *self = (struct dScMgAmida_c *)(void *)c;
    if (arg == 0) {
        int *p;
        if (self->unk_0bc >= 0x7cf)
            goto final;
        p = (int *)(((int)c + 0xbc));
        *p = *p + 1;
        if (self->unk_0bc > 0x270e)
            self->unk_0bc = 0x270e;
    } else {
        *(int *)((c + 0x5000) + 0x3e8) = 0;
        self->unk_0bc = 0;
        if (self->unk_0bc > 0x270e)
            self->unk_0bc = 0x270e;
        func_ov004_020adb1c(*(int *)((c + 0x5000) + 0x3e8));
        self->unk_0b4 = *(int *)((c + 0x5000) + 0x3e8);
    }
final:
    func_ov006_020d3ba0(c);
}
