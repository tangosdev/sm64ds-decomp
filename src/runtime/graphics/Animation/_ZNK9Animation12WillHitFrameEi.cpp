//cpp
// @symbol _ZNK9Animation12WillHitFrameEi
#include "Animation.h"

bool Animation::WillHitFrame(int frame) const
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
                return true;
        }
        else if (next >= num)
        {
            next %= num;
            if ((currFrame <= f && f < num) || f < next)
                return true;
        }
        else if (currFrame <= next)
        {
            if (currFrame <= f && f < next)
                return true;
        }
        else
        {
            if (next <= f && f < currFrame)
                return true;
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
                return true;
        }
        else if (f >= next && f < currFrame)
            return true;
    }

    return false;
}
