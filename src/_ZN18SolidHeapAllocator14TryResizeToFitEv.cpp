//cpp
// @symbol _ZN18SolidHeapAllocator14TryResizeToFitEv
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::TryResizeToFit() at 0x0204ea08 -- uses `this`.
 *
 * Shrinks the heap to exactly what has been allocated, but only when nothing has been
 * allocated backwards: the guard compares the hard end (+0x1c) against the free
 * region's end, and bails returning 0 if they differ. On success the hard end is
 * pulled down to the free region's begin and the used size is returned.
 *
 * The return is `end - this`, a byte count measured from the object itself, which is
 * why it is a subtraction of char* rather than a field read.
 */
struct H { void *begin; void *end; };

s32 SolidHeapAllocator::TryResizeToFit()
{
    struct H *base = (struct H *)((char *)this + 0x24);
    void *cur = *(void **)((char *)this + 0x1c);
    if ((s32)((char *)cur - (char *)base->end) != 0) return 0;
    *(void **)((char *)this + 0x1c) = base->begin;
    base->end = *(void **)((char *)this + 0x1c);
    return (s32)(*(char **)((char *)this + 0x1c) - (char *)this);
}
