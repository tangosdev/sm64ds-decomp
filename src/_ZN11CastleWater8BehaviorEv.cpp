//cpp
// @symbol _ZN11CastleWater8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * The whole frame: force the scroll rate, advance the texture animation.
 *
 * mTexTransformer.speed is rewritten to 0x1000 EVERY frame rather than once at init, so the
 * scroll runs at a fixed rate regardless of what else touched it.
 */
#include "CastleWater.h"


int CastleWater::Behavior()
{
    mTexTransformer.speed = 0x1000;
    ((Animation *)&mTexTransformer)->Advance();
    return 1;
}
