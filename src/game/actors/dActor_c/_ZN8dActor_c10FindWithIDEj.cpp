//cpp
// @symbol _ZN8dActor_c10FindWithIDEj
/* dActor_c::FindWithID(u32) at 0x02010c40.
 *
 * Static: searches the live-actor list rather than acting on an instance, which
 * is why it takes no `this`. The list helper returns a node, and the actor
 * itself hangs off the node at +8.
 */
#include "dActor_c.h"

extern "C" {
extern dActor_c *data_0209b468;
extern dActor_c *func_02043f98(dActor_c **head, unsigned int id);
}

dActor_c *dActor_c::FindWithID(u32 id)
{
    dActor_c *node = func_02043f98(&data_0209b468, id);
    if (node)
        return *(dActor_c **)((char *)node + 8);
    return 0;
}
