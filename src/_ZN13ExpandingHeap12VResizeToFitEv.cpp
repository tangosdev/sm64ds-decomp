//cpp
// @symbol _ZN13ExpandingHeap12VResizeToFitEv
/* ExpandingHeap::VResizeToFit() at 0x0203c388 -- Heap vtable slot 15. Eight
 * bytes: `mov r0,#0 / bx lr'.
 *
 * Always fails, and 0 is the failure value Heap::ResizeToFit tests. This is the
 * one slot where the two heaps genuinely invert: SolidHeap hands its unused tail
 * back to the parent, while an expanding heap's free space is scattered across a
 * node list and there is no tail to give. */
#include "ExpandingHeap.h"

u32 ExpandingHeap::VResizeToFit()
{
    return 0;
}
