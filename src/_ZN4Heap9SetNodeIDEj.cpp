//cpp
// @symbol _ZN4Heap9SetNodeIDEj
/* Heap::SetNodeID(u32) at 0x0203c408 -- plain forward to slot 13. The node id
 * is the tag the allocator stamps on blocks it hands out, so a later sweep can
 * free everything belonging to one id (see
 * ExpandingHeapAllocator::DeallocateAll).
 *
 * The old shadow declared its slot-13 entry as `v13(void*)' and converted the
 * incoming id to a pointer to call it -- an invented cast that existed only to
 * satisfy the invented declaration. Slot 13 takes a u32
 * (ExpandingHeap::VSetNodeID(u32 id)), so the conversion is gone. */
#include "Heap.h"

void Heap::SetNodeID(u32 id)
{
    VSetNodeID(id);
}
