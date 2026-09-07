//cpp
// @symbol _ZN10dBgCh_Actr13SetLimMovFlagEv
/* recovered: named members + shared header, real C++ method */
#include "dBgCh_Actr.h"


void dBgCh_Actr::SetLimMovFlag()
{
    *(unsigned int *)((char *)&mFlags) |= 0x80;
}
