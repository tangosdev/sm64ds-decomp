//cpp
// @symbol _ZN4dBgW9IsEnabledEv
#include "dBgW.h"

int dBgW::IsEnabled()
{
    u8 v = slotIdx;
    if (v != 0x18)
        return 1;
    return 0;
}
