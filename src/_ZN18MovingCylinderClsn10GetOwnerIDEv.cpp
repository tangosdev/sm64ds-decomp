//cpp
// @symbol _ZN18MovingCylinderClsn10GetOwnerIDEv
/* recovered: named members + shared header, real C++ method */
#include "MovingCylinderClsn.h"


/* Slot 3. owner->uniqueID, at Actor + 4 -- the same offset MeshColliderBase
   reads for its own ownerUniqueID. */
u32 MovingCylinderClsn::GetOwnerID()
{
    char *q = *(char **)&owner;
    return *(u32 *)(q + 4);
}
