//cpp
// @symbol _ZN13ExpandingHeap10VGetNodeIDEv
/* ExpandingHeap::VGetNodeID() at 0x0203c3e0 -- Heap vtable slot 14. Forwards to
 * the allocator, which really does keep an id: it stamps every node it hands
 * out, so DeallocateAll can later free just one id's worth. SolidHeap's
 * override returns a constant 0 because a linear allocator has nothing to
 * stamp. */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

u32 ExpandingHeap::VGetNodeID()
{
    return allocator->GetNodeID();
}
