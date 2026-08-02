//cpp
// @symbol _ZN9Animation8FinishedEv
#include "Animation.h"

int Animation::Finished()
{
    u32 f = numFramesAndFlags;
    int cur = currFrame;
    return cur >= (int)((f & 0x3fffffff) - 1);
}
