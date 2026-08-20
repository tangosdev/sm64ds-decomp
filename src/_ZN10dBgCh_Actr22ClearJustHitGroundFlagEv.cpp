//cpp
// @symbol _ZN10dBgCh_Actr22ClearJustHitGroundFlagEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh_Actr.h"


void dBgCh_Actr::ClearJustHitGroundFlag()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x20;
}
