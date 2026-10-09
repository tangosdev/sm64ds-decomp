//cpp
// @symbol _ZN5dBgCh19StartDetectingWaterEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh.h"


void dBgCh::StartDetectingWater()
{
    *(unsigned char *)((char *)&mDetectFlags) |= 2;
}
