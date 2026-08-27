//cpp
// @symbol _ZN4Heap28CreateExpandingHeapAllocatorEPvjj
/* Heap::CreateExpandingHeapAllocator(void*, u32, u32) at 0x0204e3c0 -- the
 * expanding twin of Heap::CreateSolidHeapAllocator, identical bar the minimum.
 * 0x4c against the solid allocator's 0x30: an expanding heap keeps a free-node
 * list, so its bookkeeping is larger. See that file for the rounding. */
#include "Heap.h"
#include "ExpandingHeapAllocator.h"

extern "C" ExpandingHeapAllocator* _ZN22ExpandingHeapAllocatorC1EPvj(
    ExpandingHeapAllocator* self, void* heapEnd, u32 flags);

ExpandingHeapAllocator* Heap::CreateExpandingHeapAllocator(void* address, u32 size, u32 flags)
{
    u32 end   = (size + (u32)address) & ~3u;
    u32 start = ((u32)address + 3u)   & ~3u;

    if (start > end || (end - start) < 0x4c)
        return 0;

    return _ZN22ExpandingHeapAllocatorC1EPvj((ExpandingHeapAllocator*)start, (void*)end, flags);
}
