//cpp
// @symbol _ZN7Tornado6State2Ev
#include "Tornado.h"

extern "C" s32 Vec3_HorzDist(const void *a, const void *b);

void Tornado::State2()
{
    s32 scale = (0x3c - mStateTimer) << 12;
    if (scale < 0)
        goto finished;

    UpdateSpin(scale / 0x3c);
    return;

finished:
    mdCcAc_c.flags |= 1;
    mCaughtActor = 0;

    Player *player = ClosestPlayer();
    if (player) {
        if (Vec3_HorzDist(&mHomePosX, (char *)player + 0x5c) > 0x9c4000)
            mState = 0;
        if (mStateTimer > 0x168)
            mState = 0;
        return;
    }
    mState = 0;
}
