//cpp
// @symbol func_ov006_020f7320
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory2_c.h"

extern "C" {
// recovered name: dScMgMemory2_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* dScMgMemory2_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern int func_ov006_020c1718(int* p);
extern void FreeGfxSlotsById(int n);

int func_ov006_020f7320(char* c) {
    struct dScMgMemory2_c *self = (struct dScMgMemory2_c *)(void *)c;
    if (self->unk_53d4 == 3 && self->unk_53d8 == 0) {
        if (func_ov006_020c1718((int*)(c + 0x4f38)) == 0) return 0;
        self->unk_53d8 = 1;
        FreeGfxSlotsById(0x1d);
    }
    return self->unk_53d4 == 4;
}
}
