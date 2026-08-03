//cpp
// @symbol _ZN5Actor10FindWithIDEj
/* Actor::FindWithID(u32) at 0x02010c40.
 *
 * Static: searches the live-actor list rather than acting on an instance, which
 * is why it takes no `this`. The list helper returns a node, and the actor
 * itself hangs off the node at +8.
 */
#include "Actor.h"

extern "C" {
extern Actor *data_0209b468;
extern Actor *func_02043f98(Actor **head, unsigned int id);
}

Actor *Actor::FindWithID(u32 id)
{
    Actor *node = func_02043f98(&data_0209b468, id);
    if (node)
        return *(Actor **)((char *)node + 8);
    return 0;
}
