//cpp
// @symbol _ZN11CastleWaterD1Ev
/* Real complete destructor. The compiler emits the CastleWater vptr store,
 * destroys TextureTransformer, then inlines dBgActor_c's destructor chain. */

#include "CastleWater.h"

CastleWater::~CastleWater()
{
}
