//cpp
/* Actor::TriplePoofDust() at 0x0200fdc8, 0x34 bytes.
 *
 * The stack-copy wrapper around TriplePoofDustAt, at the actor's own position.
 * See src/_ZN5Actor8PoofDustEv.cpp -- same shape, different worker.
 */
#include "Actor.h"

void Actor::TriplePoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    TriplePoofDustAt(pos);
}
