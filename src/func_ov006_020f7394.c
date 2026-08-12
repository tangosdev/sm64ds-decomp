// @symbol func_ov006_020f7394
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory2_c.h"
// recovered name: dScMgMemory2_c_OnYoshiTryEat_020f7394
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern void func_ov004_020b66d4(void *o);
extern char *func_020beb68;

void func_ov006_020f7394(char *c)
{
    struct dScMgMemory2_c *self = (struct dScMgMemory2_c *)(void *)c;
    char *o;
    int v;
    func_ov006_020f7064(c);
    self->unk_53d4 = 0;
    o = func_020beb68;
    v = 0;
    if (o != 0) v = *(int *)(o + 0xa8);
    if (v >= 5) v = 5;
    func_ov004_020b66d4(o);
    func_020bc7d4 = 1;
    self->unk_5409 = (unsigned char)v;
}
