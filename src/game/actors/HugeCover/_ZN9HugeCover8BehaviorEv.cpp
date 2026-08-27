//cpp
// @symbol _ZN9HugeCover8BehaviorEv
#include "HugeCover.h"

/* The ROM passes both Fix12<int> values directly in registers. Defining the
 * imported function as an ordinary C++ member homes the by-value class
 * parameters to the stack under mwccarm 2004/b56, so keep the measured ABI
 * spelling until that shared compiler wall is solved. */
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self,
                                                             int a, int b);

s32 HugeCover::Behavior()
{
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    return 1;
}
