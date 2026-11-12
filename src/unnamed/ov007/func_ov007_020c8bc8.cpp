//cpp
#include "Ov007Transition.h"
#include "nitro/hw/registers.h"
typedef long long s64;

/* Eases EVA and EVB by the Q12 progress t and writes BLDALPHA on the chosen
 * screens; effect 3 makes EVB the complement of EVA. */
extern "C" void func_ov007_020c8bc8(Ov007Transition *tr, int t) {
    unsigned char c1 = tr->fx.alpha.evaTo;
    unsigned char c0 = tr->fx.alpha.evaFrom;
    int v2 = c1;
    if (t < 0x1000) {
        if (t <= 0)
            v2 = c0;
        else
            v2 = (int)(((s64)(0x1000 - t) * (c0 << 12) + (s64)t * (c1 << 12)) >> 12) >> 12;
    }
    {
        unsigned char d1 = tr->fx.alpha.evbTo;
        unsigned char d0 = tr->fx.alpha.evbFrom;
        int vip = d1;
        if (t < 0x1000) {
            if (t <= 0)
                vip = d0;
            else
                vip = (int)(((s64)(0x1000 - t) * (d0 << 12) + (s64)t * (d1 << 12)) >> 12) >> 12;
        }
        if (tr->effect == 3)
            vip = 0x10 - v2;
        if (tr->screens & 1)
            REG_BLDALPHA = (u16)(v2 | (vip << 8));
        if (tr->screens & 2)
            REG_DB_BLDALPHA = (u16)(v2 | (vip << 8));
    }
}
