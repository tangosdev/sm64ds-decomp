//cpp
// @symbol _ZN12WithMeshClsn19StartDetectingWaterEv
/* recovered: named members + shared header, real C++ method */
#include "WithMeshClsn.h"

/* Forwards to both sub-objects' BgCh bases, the RaycastLine at 0x134 first
   and the SphereClsn at 0x20 second -- that is the ROM's order. The casts are
   what the flat field list costs: both members carry a BgCh at their own
   offset 0, but they are not modelled as BgCh-derived yet (see the header). */
void WithMeshClsn::StartDetectingWater()
{
    ((BgCh *)&mRaycastLine)->StartDetectingWater();
    ((BgCh *)&mSphereClsn)->StartDetectingWater();
}
