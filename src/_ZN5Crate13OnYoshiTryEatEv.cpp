//cpp
// @symbol _ZN5Crate13OnYoshiTryEatEv
/* recovered: named members + real C++ method */
/* Crate::OnYoshiTryEat() -- vtable slot 18. Returns 6 (edible) unless the crate
 * is already breaking: Behavior counts mBreakTimer down once a frame and poofs
 * the crate when it reaches 0, and while that is running Yoshi gets nothing. */
#include "Crate.h"

int Crate::OnYoshiTryEat()
{
    unsigned char v = mBreakTimer;
    if (v != 0) return 0;
    return 6;
}
