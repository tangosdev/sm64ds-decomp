//cpp
// @symbol _ZN12WithMeshClsn18StopDetectingWaterEv
/* recovered: named members + shared header, real C++ method */
#include "WithMeshClsn.h"

/* Mirror of StartDetectingWater: RaycastLine at 0x134 first, SphereClsn at
   0x20 second. See that file and the header for the casts. */
void WithMeshClsn::StopDetectingWater()
{
    ((BgCh *)&mRaycastLine)->StopDetectingWater();
    ((BgCh *)&mSphereClsn)->StopDetectingWater();
}
