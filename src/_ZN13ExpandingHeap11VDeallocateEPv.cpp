//cpp
// @symbol _ZN13ExpandingHeap11VDeallocateEPv
/* ExpandingHeap::VDeallocate(void*) at 0x0203c50c -- Heap vtable slot 4. Free
 * one block back to the node list.
 *
 * NULL is tolerated silently, which is what lets callers deallocate
 * unconditionally. This is the slot where the two heaps differ most plainly:
 * SolidHeap's override crashes on any non-null pointer, because a linear
 * allocator cannot return a single block. */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

void ExpandingHeap::VDeallocate(void* ptr)
{
    if (!ptr)
        return;

    allocator->Deallocate(ptr);
}
