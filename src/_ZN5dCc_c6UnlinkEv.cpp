//cpp
// @symbol _ZN5dCc_c6UnlinkEv
/* Removes this collision node from the global intrusive list. The member role
 * is proven by the prev/next rewiring; the original spelling `Unlink` is an
 * evidence-bounded reconstruction. */
#include "dCc_c.h"

extern dCc_c *data_0209cee8;

void dCc_c::Unlink()
{
    if (prev)
        prev->next = next;
    else if (data_0209cee8 == this)
        data_0209cee8 = next;

    if (next)
        next->prev = prev;

    prev = 0;
    next = 0;
}
