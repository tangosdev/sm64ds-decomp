//cpp
// @symbol _ZN9SolidHeap11VMemoryLeftEv
/* SolidHeap::VMemoryLeft() at 0x0203c598 -- Heap vtable slot 12.
 *
 * Slots 10, 11 and 12 all forward to the same MemoryLeft(4) on a solid heap:
 * the allocator is linear, so the memory left, the largest allocatable block
 * and the largest single unit are all the same number. On an expanding heap
 * they differ, which is why the three slots exist at all. */
#include "SolidHeap.h"
#include "SolidHeapAllocator.h"

u32 SolidHeap::VMemoryLeft()
{
    return allocator->MemoryLeft(4);
}
