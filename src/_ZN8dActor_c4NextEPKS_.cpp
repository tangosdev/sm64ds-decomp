//cpp
// @symbol _ZN8dActor_c4NextEPKS_
#include "dActor_c.h"

extern "C" fLiNdBa_c *data_0209b468;

/* Walks the global actor list. A null receiver means "start at the head".
   The list threads through each actor's typed node at 0x50. */
dActor_c *dActor_c::Next(const dActor_c *after)
{
    fLiNdBa_c *node;
    if (after)
        node = after->mActorListNode.next;
    else
        node = data_0209b468;

    if (node)
        return static_cast<dActor_c *>(node->owner);
    return 0;
}
