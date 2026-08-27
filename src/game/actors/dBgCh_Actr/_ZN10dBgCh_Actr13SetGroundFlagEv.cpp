//cpp
// @symbol _ZN10dBgCh_Actr13SetGroundFlagEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh_Actr.h"


void dBgCh_Actr::SetGroundFlag()
{
    *(unsigned int *)((char *)&mFlags) |= 0x10;
}
