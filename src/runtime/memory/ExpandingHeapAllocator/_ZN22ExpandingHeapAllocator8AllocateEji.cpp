//cpp
// @symbol _ZN22ExpandingHeapAllocator8AllocateEji
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::Allocate(u32 size, int align) at 0x0204e370 -- uses `this`.
 *
 * The public entry point. Two normalisations before dispatch: a zero-byte request
 * becomes one byte, and the size is rounded up to a multiple of 4.
 *
 * THE SIGN OF `align` IS THE DIRECTION, not part of the alignment. A non-negative
 * align allocates from the low end of the heap and a negative one from the high end,
 * with the magnitude used either way -- which is why the backwards call passes
 * `-align`. That is also why the parameter is `int` and not `u32`: `Eji` says
 * (unsigned int, int), and making it unsigned would delete the branch.
 *
 * AllocateForwards/AllocateBackwards keep their raw mangled names -- they are not
 * declared in the header yet, and migration is per-reference.
 */
extern "C" {
void *_ZN22ExpandingHeapAllocator17AllocateBackwardsEjj(void *self, u32 size, u32 align);
void *_ZN22ExpandingHeapAllocator16AllocateForwardsEjj(void *self, u32 size, u32 align);
}

void *ExpandingHeapAllocator::Allocate(u32 size, int align)
{
    if (size == 0) size = 1;
    size = (size + 3) & ~3u;
    if (align >= 0) {
        return _ZN22ExpandingHeapAllocator16AllocateForwardsEjj(this, size, (u32)align);
    }
    return _ZN22ExpandingHeapAllocator17AllocateBackwardsEjj(this, size, (u32)-align);
}
