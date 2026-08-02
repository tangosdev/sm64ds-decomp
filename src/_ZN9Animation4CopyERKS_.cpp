//cpp
// @symbol _ZN9Animation4CopyERKS_
#include "Animation.h"

/* Copies the three members and leaves the vptr alone. */
void Animation::Copy(const Animation &anim)
{
    numFramesAndFlags = anim.numFramesAndFlags;
    currFrame         = anim.currFrame;
    speed             = anim.speed;
}
