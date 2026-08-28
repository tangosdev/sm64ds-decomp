//cpp
// @symbol _ZNK9Animation12WillHitFrameEi
#include "Animation.h"

int Animation::WillHitFrame(int frame) const
{
    s32 f = frame << 12;
    s32 next = currFrame + speed;
    s32 num = numFramesAndFlags & ~0xc0000000;

    if ((numFramesAndFlags & 0xc0000000) == 0)
    {
        if (next < 0)
        {
            next = (next + num) % num;
            if ((f >= 0 && f < currFrame) || (next <= f && f < num))
                return 1;
        }
        else if (next >= num)
        {
            next %= num;
            if ((currFrame <= f && f < num) || f < next)
                return 1;
        }
        else if (currFrame <= next)
        {
            if (currFrame <= f && f < next)
                return 1;
        }
        else
        {
            if (next <= f && f < currFrame)
                return 1;
        }
    }
    else
    {
        if (next < 0)
            next = 0;
        if (next >= num)
            next = num - 1;

        if (currFrame <= next)
        {
            if (f >= currFrame && f < next)
                return 1;
        }
        else if (f >= next && f < currFrame)
            return 1;
    }

    return 0;
}
