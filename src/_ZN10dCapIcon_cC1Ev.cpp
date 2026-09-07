//cpp
// @symbol _ZN10dCapIcon_cC1Ev
/* The compiler supplies the dCapIcon_c vptr store. The constructor body clears
 * the intrusive links and marks the icon as not linked, exactly as in ROM. */
#include "CapIcon.h"

dCapIcon_c::dCapIcon_c()
{
    mPrev = 0;
    mNext = 0;
    mFlags |= 4;
}
