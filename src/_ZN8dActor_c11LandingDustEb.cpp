//cpp
/* dActor_c::LandingDust(bool) at 0x0200fc0c, 0x38 bytes.
 *
 * Stack-copy wrapper: copies the actor's position into a Vector3 and hands it,
 * with the raycast flag, to the worker.
 *
 * The worker it calls is HugeLandingDustAt, and that is not a slip. The `bl`
 * at 0x0200fc34 targets 0x0200fb84, which config/arm9/symbols.txt names
 * _ZN8dActor_c17HugeLandingDustAtER7Vector3b. Its sibling HugeLandingDust calls
 * LandingDustAt just as crosswise. One adjective in each pair is wrong and the
 * bytes cannot say which, so both names stand as imported. See include/dActor_c.h.
 *
 * The flag arrives in r1 and is shuffled to r2 to free r1 for `&pos`, which is
 * also what shows the wrapper forwards its own `this` untouched in r0.
 */
#include "dActor_c.h"

void dActor_c::LandingDust(bool doRaycast)
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    HugeLandingDustAt(pos, doRaycast);
}
