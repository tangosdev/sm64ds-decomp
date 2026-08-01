//cpp
// @symbol _ZN14BlueCoinSwitch16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "BlueCoinSwitch.h"
extern int _ZN5Event8ClearBitEj(unsigned int bit);

int BlueCoinSwitch::CleanupResources()
{
    _ZN5Event8ClearBitEj(mEventID);
    return 1;
}
