//cpp
/* Actor::FindExplosionActor(CylinderClsn&) at 0x02010374, 0x40 bytes.
 *
 * "Did the thing that hit me have an owner, and was the hit an explosion?" If so,
 * resolve that owner id to a live Actor; otherwise nobody.
 *
 * This and FindEgg are the same function twice over, differing only in
 * the hitFlags bit they test -- 0x2000 egg, 0x4000 explosion. The ROM has two,
 * so the tree keeps two.
 *
 * A member whose body never touches a field: the clsn arrives in r1
 * (`ldr r0, [r1, #0x24]` is the first instruction that reads anything) and r0
 * is written by every caller and read by nobody here. See include/Actor.h for
 * why that settles static-vs-member given the mangled name.
 *
 * The shadow `struct CylinderClsn` this file used to carry is replaced by the
 * real include/CylinderClsn.h: 0x24 is otherOwner, 0x20 is hitFlags.
 */
#include "Actor.h"
#include "CylinderClsn.h"

Actor *Actor::FindExplosionActor(CylinderClsn &clsn)
{
    u32 id = clsn.otherOwner;
    u32 flags;

    if (id != 0) {
        flags = clsn.hitFlags & 0x4000;
        if (flags != 0)
            return FindWithID(id);
    }
    return 0;
}
