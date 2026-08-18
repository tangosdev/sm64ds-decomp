//cpp
/* dActor_c::SmallPoofDust() at 0x0200fd40, 0x34 bytes.
 *
 * The stack-copy wrapper around DisappearPoofDustAt, at the actor's own
 * position. See src/_ZN8dActor_c8PoofDustEv.cpp -- same shape, different worker.
 *
 * The name pairs "Small" with a worker called "Disappear"; unlike the landing
 * dust pair below it these do not contradict each other, so nothing is
 * recorded beyond the observation. See include/dActor_c.h.
 */
#include "dActor_c.h"

void dActor_c::SmallPoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    DisappearPoofDustAt(pos);
}
