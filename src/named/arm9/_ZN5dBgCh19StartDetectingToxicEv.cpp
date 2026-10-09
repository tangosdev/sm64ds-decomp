//cpp
// @symbol _ZN5dBgCh19StartDetectingToxicEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh.h"


void dBgCh::StartDetectingToxic()
{
    *(unsigned char *)((char *)&mDetectFlags) |= 8;
}
