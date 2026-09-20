//cpp
// @symbol _ZN13PeachPainting8BehaviorEv
#include "PeachPainting.h"

namespace cstd { int fdiv(int a, int b); }

/* The shared ApplyOpacity declaration preserves both retail scalar arguments. */

int PeachPainting::Behavior()
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

    mModel.ApplyOpacity(mOpacity, 1);
    return 1;
}
