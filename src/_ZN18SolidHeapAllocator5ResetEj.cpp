//cpp
// @symbol _ZN18SolidHeapAllocator5ResetEj
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::Reset(u32 params) at 0x0204eb30 -- uses `this`.
 *
 * bit 0: rewind the free region's begin; bit 1: rewind its end. Passing 3 empties the
 * heap in both directions; passing 0 is a no-op the ROM still emits.
 *
 * Both callees are now real member calls rather than the two hand-spelled
 * `extern void _ZN18SolidHeapAllocator...(void*)` declarations this file used to
 * carry. That is the point of the migration: those externs were the tree's single
 * largest source of wrong-linkage risk, because a mangled name spelled by hand in a
 * C++ TU mangles a second time and the byte gate cannot see it.
 */
void SolidHeapAllocator::Reset(u32 params)
{
    if (params & 1) {
        ResetStart();
    }
    if (params & 2) {
        ResetEnd();
    }
}
