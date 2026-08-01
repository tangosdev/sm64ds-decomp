//cpp
// @symbol _ZN15FaderBrightness10SetToStartEv
/* recovered: named members + shared header, real C++ method */
#include "FaderBrightness.h"

/* Snaps the fade to its start: 0.0 in 20.12 fixed point. */
void FaderBrightness::SetToStart()
{
    currInterp = 0;
}
