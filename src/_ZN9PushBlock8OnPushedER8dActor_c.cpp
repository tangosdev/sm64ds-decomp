//cpp
// @symbol _ZN9PushBlock8OnPushedER8dActor_c
/* recovered: named members + real C++ method */
/* PushBlock::OnPushed(dActor_c &) -- slot 25. Takes the pusher's facing as its
 * own slide direction and picks the speed from the pusher's state: 2 (a dash?)
 * gets the fast 0x8000, anything else 0x4000. */
#include "PushBlock.h"

int PushBlock::OnPushed(dActor_c &other)
{
    char *r1 = (char *)&other;
    if (r1 == 0) return;
    mPrevAngleY = *(short *)(r1 + 0x8e);
    if (*(int *)(r1 + 8) == 2) mHorzSpeed = 0x8000;
    else mHorzSpeed = 0x4000;
}
