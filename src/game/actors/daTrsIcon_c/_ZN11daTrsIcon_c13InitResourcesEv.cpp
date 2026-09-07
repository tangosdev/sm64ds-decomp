//cpp
// @symbol _ZN11daTrsIcon_c13InitResourcesEv
#include "daTrsIcon_c.h"

int daTrsIcon_c::InitResources()
{
    mStarID = (param1 >> 8) & 0xf;
    mTrackStarID = -1;
    mTrackStarID = TrackStar(mStarID, 2);
    return 1;
}
