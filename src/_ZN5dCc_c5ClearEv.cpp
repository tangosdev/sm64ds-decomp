//cpp
// @symbol _ZN12CylinderClsn5ClearEv
/* recovered: named members + shared header, real C++ method */
#include "CylinderClsn.h"


void CylinderClsn::Clear()
{
    pushback.x = 0;
    pushback.y = 0;
    pushback.z = 0;
    otherOwner = 0;
    hitFlags = 0;
}
