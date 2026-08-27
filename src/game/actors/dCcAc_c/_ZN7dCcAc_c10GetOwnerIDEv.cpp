//cpp
// @symbol _ZN7dCcAc_c10GetOwnerIDEv
/* recovered: named members + shared header, real C++ method */
#include "dCcAc_c.h"


/* Slot 3. owner->uniqueID, at dActor_c + 4 -- the same offset dBgW
   reads for its own ownerUniqueID. */
u32 dCcAc_c::GetOwnerID()
{
    char *q = *(char **)&owner;
    return *(u32 *)(q + 4);
}
