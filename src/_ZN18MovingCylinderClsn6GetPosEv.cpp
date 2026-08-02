//cpp
// @symbol _ZN18MovingCylinderClsn6GetPosEv
/* recovered: named members + shared header, real C++ method */
#include "MovingCylinderClsn.h"


/* Slot 2. Returns the OWNER's position (Actor + 0x5c), not a field of this
   object -- a moving cylinder tracks its Actor instead of storing a copy. */
Vector3 & MovingCylinderClsn::GetPos()
{
    char *q = *(char **)&owner;
    return *(Vector3 *)(q + 0x5c);
}
