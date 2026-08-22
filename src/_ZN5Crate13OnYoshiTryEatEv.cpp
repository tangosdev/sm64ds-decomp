//cpp
// @symbol _ZN5Crate13OnYoshiTryEatEv
/* recovered: named members + real C++ method */
/* Crate::OnYoshiTryEat() -- vtable slot 18. Returns 6 (edible) unless unk_606
 * marks the crate as already consumed, in which case Yoshi gets nothing. */
#include "Crate.h"

int Crate::OnYoshiTryEat()
{
    unsigned char v = unk_606;
    if (v != 0) return 0;
    return 6;
}
