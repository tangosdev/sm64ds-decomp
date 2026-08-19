//cpp
/* dActor_c::FindEgg(dCc_c&) at 0x020103b4, 0x40 bytes.
 *
 * "Did the thing that hit me have an owner, and was the hit an egg strike?" If so,
 * resolve that owner id to a live dActor_c; otherwise nobody.
 *
 * This and FindExplosionActor are the same function twice over, differing only in
 * the hitFlags bit they test -- 0x2000 egg, 0x4000 explosion. The ROM has two,
 * so the tree keeps two.
 *
 * A member whose body never touches a field: the clsn arrives in r1
 * (`ldr r0, [r1, #0x24]` is the first instruction that reads anything) and r0
 * is written by every caller and read by nobody here. See include/dActor_c.h for
 * why that settles static-vs-member given the mangled name.
 *
 * The shadow `struct dCc_c` this file used to carry is replaced by the
 * real include/dCc_c.h: 0x24 is otherOwner, 0x20 is hitFlags.
 */
#include "dActor_c.h"
#include "dCc_c.h"

dActor_c *dActor_c::FindEgg(dCc_c &clsn)
{
    u32 id = clsn.otherOwner;
    u32 flags;

    if (id != 0) {
        flags = clsn.hitFlags & 0x2000;
        if (flags != 0)
            return FindWithID(id);
    }
    return 0;
}
