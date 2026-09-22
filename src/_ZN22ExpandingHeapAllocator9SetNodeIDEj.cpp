//cpp
// @symbol _ZN22ExpandingHeapAllocator9SetNodeIDEj
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::SetNodeID(u32 id) at 0x0204e0e8 -- uses `this`.
 *
 * Stamps the ID that subsequently allocated nodes carry, and returns the previous one
 * so a caller can restore it. The field is a u16 at +0x34, which is why the incoming
 * u32 is truncated on the way in and the u16 is widened to int on the way out.
 *
 * GetNodeID reads the same field and spells it `((u16 *)this)[26]` -- 0x34/2.
 *
 * THE TWO-STEP ADDRESSING IS LOAD-BEARING. Reaching the field as `Inner* p = this +
 * 0x24` then `p->id` is not the same to mwccarm as computing `this + 0x34` directly:
 * the flattened form differs by three words. Only the receiver is substituted here;
 * the body is otherwise exactly as recovered.
 */
struct Inner { char pad[0x10]; unsigned short id; };

int ExpandingHeapAllocator::SetNodeID(u32 id)
{
    struct Inner* p = (struct Inner*)((char*)this + 0x24);
    unsigned short old = p->id;
    p->id = (unsigned short)id;
    return old;
}
