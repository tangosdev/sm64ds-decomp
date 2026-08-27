//cpp
// @symbol _ZN18SolidHeapAllocator16AllocateForwardsEPvjj
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::AllocateForwards(void*, u32, u32) at 0x0204ecd4 -- STATIC.
 *
 * Takes no `this`: the mangled name declares three parameters (`EPvjj`) and the ROM
 * body takes exactly three arguments, leaving no room for one. It operates on the
 * free-region pair its caller passes -- `allocator + 0x24` -- not on the allocator.
 *
 * Carves `size` bytes off the low end: round begin up to `align`, refuse if that runs
 * past end, then push begin past the block. `align` is already a positive power of two
 * here; Allocate took the absolute value and picked the direction.
 *
 * The flags word sits 4 bytes BEFORE the pair, which is why it is read at -4 from the
 * parameter -- that is field +0x20 of the allocator. Bit 0 means fill-on-allocate, and
 * the fill covers begin..end, including the alignment gap, not just the returned block.
 */
extern "C" void MultiStore_Int(s32 val, void *dst, u32 count);

void *SolidHeapAllocator::AllocateForwards(void *freeBlockPair, u32 size, u32 align)
{
    void **pair = (void **)freeBlockPair;
    void *freeBlockBegin = pair[0];
    void *start = (void *)(((align - 1) + (u32)freeBlockBegin) & ~(align - 1));
    void *freeBlockEnd = pair[1];
    void *end = (void *)((u32)size + (u32)start);
    if (end > freeBlockEnd) {
        return (void *)0;
    }
    u32 flags = *(u32 *)((char *)freeBlockPair - 4);
    u32 fillSize = (u32)end - (u32)freeBlockBegin;
    if ((u16)(flags & 0xff) & 1) {
        volatile s32 zero = 0;
        MultiStore_Int(zero, freeBlockBegin, fillSize);
    }
    pair[0] = end;
    return start;
}
