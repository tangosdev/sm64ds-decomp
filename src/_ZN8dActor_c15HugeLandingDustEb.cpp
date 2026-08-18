//cpp
/* Actor::HugeLandingDust(bool) at 0x0200fb4c, 0x38 bytes.
 *
 * The other half of the crossed pair: the `bl` at 0x0200fb74 targets
 * 0x0200fac4 = _ZN5Actor13LandingDustAtER7Vector3b. See
 * src/_ZN5Actor11LandingDustEb.cpp and include/Actor.h.
 */
#include "Actor.h"

void Actor::HugeLandingDust(bool doRaycast)
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    LandingDustAt(pos, doRaycast);
}
