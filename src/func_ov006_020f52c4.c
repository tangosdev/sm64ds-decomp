// @symbol func_ov006_020f52c4
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory_c.h"
// @emits dScMgMemory_c_OnYoshiTryEat_020f52c4
/* recovered: renamed to Class_Method */
/* dScMgMemory_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void func_ov004_020b66d4(void *o);
extern char *func_020beb68;

void dScMgMemory_c_OnYoshiTryEat_020f52c4(char *c)
{
    struct dScMgMemory_c *self = (struct dScMgMemory_c *)(void *)c;
    char *o;
    int v;
    func_ov006_020f4f94(c);
    self->unk_5314 = 0;
    o = func_020beb68;
    v = 0;
    if (o != 0) v = *(int *)(o + 0xa8);
    if (v >= 3) v = 3;
    func_ov004_020b66d4(o);
    func_020bc7d4 = 1;
    self->unk_533b = (unsigned char)v;
}
