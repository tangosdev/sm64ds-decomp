//cpp
// @symbol _ZN5dBgCh21StopDetectingOrdinaryEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh.h"


void dBgCh::StopDetectingOrdinary()
{
    *(unsigned char *)((char *)&mDetectFlags) &= ~1;
}
