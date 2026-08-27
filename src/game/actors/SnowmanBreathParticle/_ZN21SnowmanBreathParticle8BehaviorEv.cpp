//cpp
// @symbol _ZN21SnowmanBreathParticle8BehaviorEv
#include "SnowmanBreath.h"

extern "C" int DecIfAbove0_Byte(void *);

void SnowmanBreathParticle::Behavior()
{
    if (DecIfAbove0_Byte(&mTimer) == 0)
        return;
    UpdatePosition();
    CheckSnowman();
    HitPlayer();
    mCollider.pos = mPos;
    mCollider.Clear();
    mCollider.Update();
}
