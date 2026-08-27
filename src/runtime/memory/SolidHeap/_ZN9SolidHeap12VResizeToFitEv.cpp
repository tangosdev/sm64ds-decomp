//cpp
// @symbol _ZN9SolidHeap12VResizeToFitEv
/* SolidHeap::VResizeToFit() at 0x0203c33c -- Heap vtable slot 15. Give back the
 * arena's unused tail.
 *
 * Two steps, and the second can fail after the first has succeeded. First the
 * allocator reports how much it actually needs; then this heap shrinks its own
 * block in the PARENT heap to that much plus 0x18 -- the SolidHeap object
 * itself, which lives at the front of the block. Only once the parent has
 * agreed is heapSize updated.
 *
 * Returns the new total block size, or 0 if either step refused.
 *
 * This file used to re-declare `struct Heap' and `struct SolidHeap' locally and
 * reach the base through `thiz->base.parentHeap'. It is the same layout, said
 * once. Its call to Heap::Reallocate is also what proved that method returns a
 * value at all -- see _ZN4Heap10ReallocateEPvj.cpp. */
#include "SolidHeap.h"
#include "SolidHeapAllocator.h"

u32 SolidHeap::VResizeToFit()
{
    u32 used = allocator->TryResizeToFit();
    if (!used)
        return 0;

    u32 total = used + 0x18;
    if (!parentHeap->Reallocate(this, total))
        return 0;

    heapSize = used;
    return total;
}
