//cpp
// @symbol _ZN13dScMgMemory_c13OnTurnIntoEggEi
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory_c.h"

extern "C" {
// recovered name: dScMgMemory_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* dScMgMemory_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern int func_ov006_020c1718(int* p);
extern void FreeGfxSlotsById(int n);

int dScMgMemory_c::OnTurnIntoEgg(int /* mode */)
{
    char *c = (char *)this;

    struct dScMgMemory_c *self = (struct dScMgMemory_c *)(void *)c;
    if (self->unk_5314 == 3 && self->unk_5318 == 0) {
        if (func_ov006_020c1718((int*)(c + 0x4f38)) == 0) return 0;
        self->unk_5318 = 1;
        FreeGfxSlotsById(0x1d);
    }
    return self->unk_5314 == 4;
}
}
