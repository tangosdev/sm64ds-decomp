//cpp
// @symbol _ZN15FaderBrightness9IsAtStartEv
/* recovered: named members + shared header, real C++ method */
#include "FaderBrightness.h"

int FaderBrightness::IsAtStart()
{
    return currInterp == 0;
}
