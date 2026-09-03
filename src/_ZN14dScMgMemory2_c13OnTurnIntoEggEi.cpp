//cpp
// @symbol _ZN14dScMgMemory2_c13OnTurnIntoEggEi
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory2_c.h"

extern "C" {
// recovered name: dScMgMemory2_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* dScMgMemory2_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern int func_ov006_020c1718(int* p);
extern void FreeGfxSlotsById(int n);

int dScMgMemory2_c::OnTurnIntoEgg(int /* mode */)
{
    char *c = (char *)this;

    if (mState == 3 && mSubstate == 0) {
        if (func_ov006_020c1718((int*)(c + 0x4f38)) == 0) return 0;
        mSubstate = 1;
        FreeGfxSlotsById(0x1d);
    }
    return mState == 4;
}
}
