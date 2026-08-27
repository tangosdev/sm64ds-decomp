//cpp
// @symbol _ZN4Heap24CreateSolidHeapAllocatorEPvjj
/* Heap::CreateSolidHeapAllocator(void*, u32, u32) at 0x0204ebc4 -- fit a
 * SolidHeapAllocator into [address, address + size), or refuse.
 *
 * Both ends are brought to a 4-byte boundary, inwards: the start rounds up and
 * the end rounds down, so the aligned span is never larger than the span asked
 * for. The refusal has two halves and both matter -- `start > end' catches the
 * case where rounding crossed the ends over each other on a tiny or misaligned
 * block, and 0x30 is the allocator's own bookkeeping, below which there would
 * be nothing left to hand out.
 *
 * The allocator is constructed AT start_u: the object lives inside the block it
 * manages. That is why the constructor takes the end as its argument and needs
 * no separate size. */
#include "Heap.h"
#include "SolidHeapAllocator.h"

extern "C" SolidHeapAllocator* _ZN18SolidHeapAllocatorC1EPvj(
    SolidHeapAllocator* self, void* heapEnd, u32 flags);

SolidHeapAllocator* Heap::CreateSolidHeapAllocator(void* address, u32 size, u32 flags)
{
    u32 end   = (size + (u32)address) & ~3u;
    u32 start = ((u32)address + 3u)   & ~3u;

    if (start > end || (end - start) < 0x30)
        return 0;

    return _ZN18SolidHeapAllocatorC1EPvj((SolidHeapAllocator*)start, (void*)end, flags);
}
