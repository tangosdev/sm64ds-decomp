//cpp
// @symbol _ZN15FaderBrightness8SetToEndEv
/* recovered: named members + shared header, real C++ method */
#include "FaderBrightness.h"

/* Snaps the fade to its end: 1.0 in 20.12 fixed point. */
void FaderBrightness::SetToEnd()
{
    currInterp = 0x1000;
}
