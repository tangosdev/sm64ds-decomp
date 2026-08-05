//cpp
// @symbol _ZN14EnemySwitchTag16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "EnemySwitchTag.h"
extern "C" {
extern int _ZN5Event8ClearBitEj(unsigned int bit);
}

int EnemySwitchTag::CleanupResources()
{
    _ZN5Event8ClearBitEj(mEventID);
    return 1;
}
