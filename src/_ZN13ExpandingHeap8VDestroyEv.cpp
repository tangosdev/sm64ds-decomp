//cpp
// @symbol _ZN13ExpandingHeap8VDestroyEv
/* ExpandingHeap::VDestroy() at 0x0203c72c -- Heap vtable slot 2. Tear the
 * allocator down and forget it; Heap::Destroy calls this before returning the
 * block to the parent heap.
 *
 * The call goes to HeapAllocator::Destroy(), not a derived override. The typed
 * ExpandingHeapAllocator inheritance records that base relationship directly;
 * SolidHeap::VDestroy's parallel but still-unnamed target remains raw. */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

void ExpandingHeap::VDestroy()
{
    allocator->Destroy();
    allocator = 0;
}
