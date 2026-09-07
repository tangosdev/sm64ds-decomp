//cpp
// @symbol _ZN5dBgCh18StopDetectingWaterEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh.h"


void dBgCh::StopDetectingWater()
{
    *(unsigned char *)((char *)&mDetectFlags) &= ~2;
}
