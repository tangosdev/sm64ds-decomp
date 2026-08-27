//cpp
// @symbol _ZN18SolidHeapAllocator10ReallocateEPvj
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::Reallocate(void* ptr, u32 size) at 0x0204e964 -- uses `this`.
 *
 * Resizes IN PLACE, and only for the most recent forward allocation: `curSize` is
 * measured as `freeRegion.begin - ptr`, so the block being resized is assumed to end
 * exactly where the free region starts. Growing anything else silently moves the
 * boundary. Returns the new size, or 0 if growing would overrun the free region's end.
 *
 * Shrinking cannot fail and skips the fill entirely -- the guard is `size > curSize`,
 * not a general fill -- so the flags word (+0x20, bit 0) only matters when growing,
 * and then only the newly exposed bytes are cleared.
 */
extern "C" void MultiStore_Int(s32 val, void *dst, u32 count);

u32 SolidHeapAllocator::Reallocate(void *ptr, u32 size)
{
    void **fl = (void **)((char *)this + 0x24);
    if (size == 0) size = 1;
    size = (size + 3) & ~3;
    void *begin = fl[0];
    u32 newEnd = size + (u32)ptr;
    u32 curSize = (u32)begin - (u32)ptr;
    if (size == curSize) {
        return size;
    }
    if (size > curSize) {
        if ((s32)(newEnd - (u32)fl[1]) > 0) {
            return 0;
        }
        u32 flags = *(u32 *)((char *)this + 0x20);
        if ((u16)(flags & 0xff) & 1) {
            volatile s32 zero = 0;
            MultiStore_Int(zero, begin, size - curSize);
        }
    }
    fl[0] = (void *)newEnd;
    return size;
}
