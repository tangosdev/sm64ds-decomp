//cpp
// @symbol _ZN9SolidHeap10VSetNodeIDEj
/* SolidHeap::VSetNodeID(u32) at 0x0203c3f0 -- Heap vtable slot 13. Eight bytes:
 * `mov r0,#0 / bx lr'. The id is accepted and dropped; a linear allocator has
 * nothing to stamp it on.
 *
 * THIS FUNCTION IS WHY SLOT 13 RETURNS u32 rather than void. The `mov r0,#0'
 * is a materialized return value, and a void declaration cannot emit it --
 * which makes this the one place in the family where the slot's return type is
 * byte-observable. ExpandingHeap's override is a pure forward and would match
 * under either spelling, so it could not settle the question; this one does.
 *
 * The previous text opened with `u32 id = (u32)id_;', converting the parameter
 * to its own type and then never using it. */
#include "SolidHeap.h"

u32 SolidHeap::VSetNodeID(u32 id)
{
    return 0;
}
