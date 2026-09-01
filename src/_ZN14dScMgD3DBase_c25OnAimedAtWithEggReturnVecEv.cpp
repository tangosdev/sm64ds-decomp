//cpp
// @symbol _ZN14dScMgD3DBase_c25OnAimedAtWithEggReturnVecEv
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgD3DBase_c.h"

extern "C" {
// recovered name: dScMgTrampoline2_c_OnAimedAtWithEggReturnVec  -- WRONG, see below
/* recovered: renamed to Class_Method */
/* dScMgD3DBase_c::OnAimedAtWithEggReturnVec - slot 30.

   The `recovered name:` line above is kept visible because it is wrong and the
   correction is worth reading.  This address backs FIVE vtables --
   _ZTV14dScMgD3DBase_c and the four children that inherit it, dScMgJump_c,
   dScMgJump2_c, dScMgTrampoline_c and dScMgTrampoline2_c -- so it belongs to
   the base of that group, not to the last child that happened to name it.
   That is the seventh such misattribution on this class.  It also included
   dScMgTrampoline2_c.h and cast `this` to that type; both are fixed here.

   It is a real body, not a veneer: it hands the sub-screen BG and OBJ VRAM
   banks back before delegating to dScMgBase_c's, which is the half of the
   menu-down sequence that only the 3D minigames need. */
void _ZN2GX15SetBankForSubBGEt(unsigned int);
void _ZN2GX16SetBankForSubOBJEt(unsigned int);
void func_ov006_020e740c(void);
void _ZN11dScMgBase_c25OnAimedAtWithEggReturnVecEv(void *p);
void func_ov006_020e7508(void);
void func_ov006_020e759c(void);
extern unsigned char data_0209e660;
extern unsigned char data_0209f5f8;

void dScMgD3DBase_c::OnAimedAtWithEggReturnVec()
{
    char *c = (char *)this;

    struct dScMgD3DBase_c *self = (struct dScMgD3DBase_c *)(void *)c;
    _ZN2GX15SetBankForSubBGEt(self->unk_0a0);
    _ZN2GX16SetBankForSubOBJEt(self->unk_4660);
    data_0209e660 = 1;
    func_ov006_020e740c();
    _ZN11dScMgBase_c25OnAimedAtWithEggReturnVecEv(c);
    if (self->unk_4664 == 1) {
        func_ov006_020e7508();
    } else {
        func_ov006_020e759c();
        if (data_0209f5f8 == 0) data_0209f5f8 = 1;
    }
}
}
