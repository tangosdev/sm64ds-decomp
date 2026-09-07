//cpp
// @symbol _ZN10dBgCh_Actr18StopDetectingWaterEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh_Actr.h"

/* Mirror of StartDetectingWater: dBgCh_Lin at 0x134 first, dBgCh_SphCrr at
   0x20 second. See that file and the header for the casts. */
void dBgCh_Actr::StopDetectingWater()
{
    ((dBgCh *)&mRaycastLine)->StopDetectingWater();
    ((dBgCh *)&mSphereClsn)->StopDetectingWater();
}
