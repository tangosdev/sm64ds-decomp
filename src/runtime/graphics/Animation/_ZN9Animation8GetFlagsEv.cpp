//cpp
// @symbol _ZN9Animation8GetFlagsEv
#include "Animation.h"

/* The top two bits of numFramesAndFlags: LOOP is 0, NO_LOOP 0x40000000. */
int Animation::GetFlags()
{
    return numFramesAndFlags & 0xC0000000;
}
