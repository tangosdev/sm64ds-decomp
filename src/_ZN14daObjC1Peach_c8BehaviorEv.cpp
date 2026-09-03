//cpp
// @symbol _ZN14daObjC1Peach_c8BehaviorEv
#include "daObjC1Peach_c.h"

namespace cstd { int fdiv(int a, int b); }

/* The ROM call carries a third register argument even though the imported
 * mangled name records only the opacity. Keep that measured ABI view until
 * the declaration itself is repaired; the ordinary member call is shorter. */
extern "C" void _ZN9ModelBase12ApplyOpacityEj(void *, u32 opacity, int enable);

int daObjC1Peach_c::Behavior()
{
    int distance = DistToCPlayer();
    if (distance >= 0xe10000) {
        mOpacity = 0xff;
    } else if (distance <= 0xbf4000) {
        mOpacity = 0;
    } else {
        int fraction = cstd::fdiv(distance - 0xbf4000, 0x21c000);
        int opacity = (int)(((long long)fraction * 0xff + 0x800) >> 12);
        mOpacity = (u8)(opacity >> 3);
    }

    _ZN9ModelBase12ApplyOpacityEj(&mModel, mOpacity, 1);
    return 1;
}
