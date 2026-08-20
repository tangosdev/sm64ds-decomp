//cpp
// @symbol _ZN10dBgCh_Actr15ClearLimMovFlagEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh_Actr.h"


void dBgCh_Actr::ClearLimMovFlag()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x80;
}
