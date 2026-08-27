//cpp
// @symbol _ZN15FaderBrightness20IsBetweenStartAndEndEv
/* recovered: named members + shared header, real C++ method */
#include "FaderBrightness.h"

/* True while the fade is in motion: neither snapped to 0.0 nor to 1.0. Both
   calls go through the vtable (slots 5 and 6), which is how the ROM reaches
   them and how the slot assignment in Fader.h was established. */
int FaderBrightness::IsBetweenStartAndEnd()
{
    if (IsAtStart() == 0 && IsAtEnd() == 0)
        return 1;
    return 0;
}
