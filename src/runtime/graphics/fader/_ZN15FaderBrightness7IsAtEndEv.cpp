//cpp
// @symbol _ZN15FaderBrightness7IsAtEndEv
/* recovered: named members + shared header, real C++ method */
#include "FaderBrightness.h"

int FaderBrightness::IsAtEnd()
{
    return currInterp == 0x1000;
}
