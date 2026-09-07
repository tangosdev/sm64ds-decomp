//cpp
// @symbol _ZN10dBgCh_Actr15ClearGroundFlagEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh_Actr.h"


void dBgCh_Actr::ClearGroundFlag()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x10;
}
