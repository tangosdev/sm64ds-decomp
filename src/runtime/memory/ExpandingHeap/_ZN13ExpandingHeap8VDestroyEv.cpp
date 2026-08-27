//cpp
// @symbol _ZN13ExpandingHeap8VDestroyEv
/* ExpandingHeap::VDestroy() at 0x0203c72c -- Heap vtable slot 2. Tear the
 * allocator down and forget it; Heap::Destroy calls this before returning the
 * block to the parent heap.
 *
 * The call goes to _ZN13HeapAllocator7DestroyEv -- HeapAllocator, not
 * ExpandingHeapAllocator. That is a real signal about the allocator hierarchy
 * and not a mistake in the original recovery: 0x0204e3b4 is a named symbol, and
 * SolidHeap::VDestroy calls the parallel but still-unnamed func_0204ebb8. It is
 * left as a raw call because ExpandingHeapAllocator has not been shown to derive
 * from HeapAllocator -- include/HeapAllocator.h is still a rung-0 skeleton
 * declaring only Remove(), so `allocator->Destroy()' has nothing to bind to.
 * Establishing that hierarchy is its own slice; migration is per-reference. */
#include "ExpandingHeap.h"

extern "C" void _ZN13HeapAllocator7DestroyEv(ExpandingHeapAllocator* allocator);

void ExpandingHeap::VDestroy()
{
    _ZN13HeapAllocator7DestroyEv(allocator);
    allocator = 0;
}
