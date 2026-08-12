// @symbol func_ov006_020f8c68
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMCarlo_c.h"
// recovered name: dScMgMCarlo_c_OnYoshiTryEat_020f8c68
/* recovered: renamed to Class_Method */
/* dScMgMCarlo_c::OnYoshiTryEat - recovered from vtable slot identity */
void func_ov006_020f7c10(char* p);
void func_ov006_020c1604(char* c, int unused, short a2, int a3);
void func_ov004_020b66d4(char* p);


void func_ov006_020f8c68(char* c)
{
    struct dScMgMCarlo_c *self = (struct dScMgMCarlo_c *)(void *)c;
    func_ov006_020f7c10(c + 0x51a8);
    data_ov006_0213d564 = 0;
    self->unk_60ae = 0;
    self->unk_511e = 1;
    func_ov006_020c1604(c + 0x4f38, 4, 4, (int)(c + 0x60ae));
    self->unk_4f52 = 1;
    self->unk_60aa = 0;
    self->unk_60a8 = 1;
    func_ov004_020b66d4(c + 0x6000);
    func_020bc7d4 = 1;
}
