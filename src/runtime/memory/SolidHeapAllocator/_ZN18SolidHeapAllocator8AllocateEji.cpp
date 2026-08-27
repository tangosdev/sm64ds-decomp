//cpp
// @symbol _ZN18SolidHeapAllocator8AllocateEji
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::Allocate(u32 size, int align) at 0x0204eb70 -- uses `this`.
 *
 * ALIGN'S SIGN IS THE DIRECTION, not part of the alignment: non-negative allocates
 * from the low end of the free region, negative from the high end. That is why the
 * parameter is `int` and not `u32` -- making it unsigned deletes the branch. The same
 * convention holds in ExpandingHeapAllocator::Allocate, and it is the reason two
 * separate AllocateForwards/AllocateBackwards helpers exist at all.
 *
 * A zero size is rounded up to 1 so every allocation has a distinct address, then the
 * size is rounded up to a multiple of 4.
 *
 * The helpers take the free region (`this + 0x24`) rather than `this`, which is what
 * makes them static -- see the header.
 */
void *SolidHeapAllocator::Allocate(u32 size, int align)
{
    void *fb = (char *)this + 0x24;
    if (size == 0) size = 1;
    size = (size + 3) & ~3;
    if (align >= 0) {
        return AllocateForwards(fb, size, (u32)align);
    }
    return AllocateBackwards(fb, size, (u32)(-align));
}
