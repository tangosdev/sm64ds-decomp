//cpp
// @symbol _ZN4BgCh18StopDetectingWaterEv
/* recovered: named members + shared header, real C++ method */
#include "BgCh.h"


void BgCh::StopDetectingWater()
{
    *(unsigned char *)((char *)&unk_004) &= ~2;
}
