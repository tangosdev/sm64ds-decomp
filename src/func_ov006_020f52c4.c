// @symbol func_ov006_020f52c4
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory_c.h"
// recovered name: dScMgMemory_c_OnYoshiTryEat_020f52c4
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern void func_ov004_020b66d4(void *o);
extern char *func_020beb68;

void func_ov006_020f52c4(char *c)
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
