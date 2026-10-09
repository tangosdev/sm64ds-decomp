//cpp
// @symbol _ZN4Heap9SetNodeIDEj
/* Heap::SetNodeID(u32) at 0x0203c408 -- plain forward to slot 13. The node id
 * is the tag the allocator stamps on blocks it hands out, so a later sweep can
 * free everything belonging to one id (see
 * ExpandingHeapAllocator::DeallocateAll).
 *
 * Slot 13 returns u32 -- SolidHeap's override is eight bytes of `mov r0,#0 /
 * bx lr', which a void declaration could not emit. This wrapper drops it, so
 * the call is still a plain tail call and the bytes do not care.
 *
 * The old shadow declared its slot-13 entry as `v13(void*)' and converted the
 * incoming id to a pointer to call it -- an invented cast that existed only to
 * satisfy the invented declaration. */
#include "Heap.h"

void Heap::SetNodeID(u32 id)
{
    VSetNodeID(id);
}
