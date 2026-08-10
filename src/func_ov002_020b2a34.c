// @symbol func_ov002_020b2a34
// recovered name: Coin_OnTurnIntoEgg
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daCoin_c::OnTurnIntoEgg - recovered from vtable slot identity */

void func_ov002_020b2a34(char* c, char* p)
{
    int state = *(int*)(c + 0x3a0);
    if (state == 1) {
        *(int*)(c + 0x60) += 0x50000;
        func_ov002_020b16c4(c, p);
        return;
    }
    if (state == 2) {
        func_ov002_020b1674(c, p);
        return;
    }
    func_ov002_020b1884(c, p);
}
