//cpp
// @symbol _ZN9SolidHeap7VIntactEv
/* SolidHeap::VIntact() at 0x0203c654 -- Heap vtable slot 6. Eight bytes:
 * `mov r0,#1 / bx lr'.
 *
 * It answers "yes" unconditionally, so it is not a corruption check -- a linear
 * allocator has no free list to walk and nothing to find inconsistent. Note
 * what that costs the caller: Heap::Intact latches a global the first time any
 * heap reports damage, and a solid heap can never trip it. */
#include "SolidHeap.h"

bool SolidHeap::VIntact()
{
    return 1;
}
