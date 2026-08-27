//cpp
// @symbol _ZN13ExpandingHeap11VMemoryLeftEv
/* ExpandingHeap::VMemoryLeft() at 0x0203c5ac -- Heap vtable slot 12. The SUM of
 * every free node's size.
 *
 * Slots 10, 11 and 12 are three different numbers on an expanding heap and one
 * number on a solid one. Here 12 sums the free list while 10 and 11 ask for the
 * largest single node -- so a fragmented heap can report plenty of memory left
 * and still refuse an allocation. */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

u32 ExpandingHeap::VMemoryLeft()
{
    return allocator->MemoryLeft();
}
