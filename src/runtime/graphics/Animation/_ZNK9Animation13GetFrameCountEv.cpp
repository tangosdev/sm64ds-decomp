//cpp
// @symbol _ZNK9Animation13GetFrameCountEv
#include "Animation.h"

u32 Animation::GetFrameCount() const
{
    u32 v = numFramesAndFlags;
    return ((v & 0x3fffffff) << 4) >> 16;
}
