//cpp
// @symbol _ZN22ExpandingHeapAllocator10MemoryLeftEv
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::MemoryLeft() at 0x0204e180 -- uses `this`.
 *
 * Total free bytes: walk the free-node list at this+0x24 and sum each node's size
 * (+4), following the next link (+0xc). This is the sum, NOT the largest allocatable
 * block -- MaxAllocatableSize answers that, and the two differ by fragmentation.
 *
 * The offsets stay raw because the header's padding does not name the node list yet;
 * naming it is a header job and would let this read `for (n = mFreeList; ...)`.
 */
u32 ExpandingHeapAllocator::MemoryLeft()
{
    void* n = *(void**)((char*)this + 0x24);
    unsigned int total = 0;
    while (n) {
        total += *(unsigned int*)((char*)n + 4);
        n = *(void**)((char*)n + 0xc);
    }
    return total;
}
