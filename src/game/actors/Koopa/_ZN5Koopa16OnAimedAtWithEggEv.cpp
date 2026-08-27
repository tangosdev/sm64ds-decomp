//cpp
// @symbol _ZN5Koopa16OnAimedAtWithEggEv
#include "Koopa.h"
// recovered name: Koopa_OnAimedAtWithEgg
/* recovered: renamed to Class_Method */
/* daNknk_c::OnAimedAtWithEgg - recovered from vtable slot identity */
s32 Koopa::OnAimedAtWithEgg() {
    void * c = (void *)this;
    unsigned short v = *(unsigned short*)((char*)c + 0xc);
    int r;
    if (v == 0xcb) r = 1; else r = 0;
    if (r != 0) r = 0x46000; else r = 0x25800;
    return r;
}
