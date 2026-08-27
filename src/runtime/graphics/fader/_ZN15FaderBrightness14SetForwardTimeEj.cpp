//cpp
// @symbol _ZN15FaderBrightness14SetForwardTimeEj
/* recovered: named members + shared header, real C++ method */
#include "FaderBrightness.h"

extern "C" Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);

/* Fade in over `frames` frames: speed = 1.0 / frames. 0 means "instant". */
int FaderBrightness::SetForwardTime(u32 frames)
{
    if (frames == 0) {
        speed = (Fix12i)0x1000;
    } else {
        speed = _ZN4cstd4fdivEii(0x1000, (Fix12i)(frames << 12));
    }
    return IsAtEnd();
}
