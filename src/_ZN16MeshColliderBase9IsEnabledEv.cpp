//cpp
// @symbol _ZN16MeshColliderBase9IsEnabledEv
#include "MeshColliderBase.h"

int MeshColliderBase::IsEnabled()
{
    u8 v = slotIdx;
    if (v != 0x18)
        return 1;
    return 0;
}
