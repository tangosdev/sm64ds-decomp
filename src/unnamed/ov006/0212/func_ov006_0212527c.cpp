//cpp
// @symbol func_ov006_0212527c
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgBSC_c.h"
// recovered name: dScMgBSC_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* dScMgBSC_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern "C" int func_ov006_020c1718(int* r0);
extern "C" void FreeGfxSlotsById(int arg);

extern short data_ov004_020bf9e4;

extern "C" int func_ov006_0212527c(char* c, int mode)
{
    struct dScMgBSC_c *self = (struct dScMgBSC_c *)(void *)c;
    int st;
    short fl;
    if (self->unk_51b8 == 0xd) {
        if (func_ov006_020c1718((int*)(c + 0x4f38)) != 0) {
            if (mode == 4) {
                FreeGfxSlotsById(0x1d);
                self->unk_51b8 = 9;
            } else if (mode == 5) {
                FreeGfxSlotsById(0x1d);
                self->unk_51b8 = 0xa;
            }
        } else {
            return 0;
        }
    }
    st = self->unk_51b8;
    if (st >= 0xc) {
        fl = data_ov004_020bf9e4;
        if (fl == 1) goto do1;
    }
    return 0;
do1:
    if (st == 0xc && fl == 1) {
        func_ov004_020ad79c(self->unk_0a8, self->mHudScore);
    }
    return 1;
}
