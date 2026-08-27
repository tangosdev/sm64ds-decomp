//cpp
// @symbol _ZN9Animation8SetFlagsEi
#include "Animation.h"

void Animation::SetFlags(int flags)
{
    numFramesAndFlags = (numFramesAndFlags & 0x3fffffff) | flags;
}
