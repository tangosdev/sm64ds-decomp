//cpp
// @symbol _ZN10dBgCh_Actr19StartDetectingWaterEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh_Actr.h"

/* Forwards to both sub-objects' dBgCh bases, the dBgCh_Lin at 0x134 first
   and the dBgCh_SphCrr at 0x20 second -- that is the ROM's order. The casts are
   what the flat field list costs: both members carry a dBgCh at their own
   offset 0, but they are not modelled as dBgCh-derived yet (see the header). */
void dBgCh_Actr::StartDetectingWater()
{
    ((dBgCh *)&mRaycastLine)->StartDetectingWater();
    ((dBgCh *)&mSphereClsn)->StartDetectingWater();
}
