//cpp
// @symbol _ZN10dBgCh_Actr19ClearAllGroundFlagsEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh_Actr.h"


void dBgCh_Actr::ClearAllGroundFlags()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x70;
}
