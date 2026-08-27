//cpp
// @symbol _ZN18SolidHeapAllocator17AllocateBackwardsEPvjj
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::AllocateBackwards(void*, u32, u32) at 0x0204ec58 -- STATIC.
 *
 * The mirror of AllocateForwards, and static for the same reason: three declared
 * parameters, three arguments in the ROM body, no room for a `this`.
 *
 * Carves `size` bytes off the HIGH end: subtract from end, round the result DOWN to
 * `align`, refuse if that runs below begin, then pull end down. Because the rounding
 * is downward the alignment gap lands above the block rather than below it, so the
 * returned pointer is also the new end -- which is why `start` is both stored and
 * returned, where the forward case stores `end` and returns `start`.
 */
extern "C" void MultiStore_Int(s32 val, void *dst, u32 count);

void *SolidHeapAllocator::AllocateBackwards(void *freeBlockPair, u32 size, u32 align)
{
    void **pair = (void **)freeBlockPair;
    void *freeBlockEnd = pair[1];
    void *freeBlockBegin = pair[0];
    void *start = (void *)(((u32)freeBlockEnd - size) & ~(align - 1));
    if (start < freeBlockBegin) {
        return (void *)0;
    }
    u32 flags = *(u32 *)((char *)freeBlockPair - 4);
    u32 fillSize = (u32)freeBlockEnd - (u32)start;
    if ((u16)(flags & 0xff) & 1) {
        volatile s32 zero = 0;
        MultiStore_Int(zero, start, fillSize);
    }
    pair[1] = start;
    return start;
}
