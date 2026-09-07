//cpp
// @symbol _ZN4Heap21MaxAllocationUnitSizeEv
/* Heap::MaxAllocationUnitSize() at 0x0203c60c -- plain forward to slot 10.
 *
 * Was a cast of `this' to a local `struct Base' with ten anonymous virtuals
 * padding `m' out to index 10. */
#include "Heap.h"

u32 Heap::MaxAllocationUnitSize()
{
    return VMaxAllocationUnitSize();
}
