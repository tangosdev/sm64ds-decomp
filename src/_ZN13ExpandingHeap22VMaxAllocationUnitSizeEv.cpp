//cpp
// @symbol _ZN13ExpandingHeap22VMaxAllocationUnitSizeEv
/* ExpandingHeap::VMaxAllocationUnitSize() at 0x0203c5f8 -- Heap vtable slot 10.
 * Same call as slot 11 on this heap: the largest allocatable block and the
 * largest allocation unit are both "the biggest free node". */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

u32 ExpandingHeap::VMaxAllocationUnitSize()
{
    return allocator->MaxAllocatableSize(4);
}
