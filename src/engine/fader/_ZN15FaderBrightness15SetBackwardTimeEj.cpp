//cpp
// @symbol _ZN15FaderBrightness15SetBackwardTimeEj
/* recovered: named members + shared header, real C++ method */
#include "FaderBrightness.h"

extern "C" Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);

/* Fade out over `frames` frames: the same rate with the sign flipped. */
int FaderBrightness::SetBackwardTime(u32 frames)
{
    if (frames == 0) {
        speed = -(Fix12i)0x1000;
    } else {
        speed = _ZN4cstd4fdivEii(-(Fix12i)0x1000, (Fix12i)(frames << 12));
    }
    return IsAtStart();
}
