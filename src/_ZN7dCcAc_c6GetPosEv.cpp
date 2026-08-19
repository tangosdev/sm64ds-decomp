//cpp
// @symbol _ZN7dCcAc_c6GetPosEv
/* recovered: named members + shared header, real C++ method */
#include "dCcAc_c.h"


/* Slot 2. Returns the OWNER's position (dActor_c + 0x5c), not a field of this
   object -- a moving cylinder tracks its dActor_c instead of storing a copy. */
Vector3 & dCcAc_c::GetPos()
{
    char *q = *(char **)&owner;
    return *(Vector3 *)(q + 0x5c);
}
