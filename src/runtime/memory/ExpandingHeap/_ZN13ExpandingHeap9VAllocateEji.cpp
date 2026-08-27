//cpp
// @symbol _ZN13ExpandingHeap9VAllocateEji
/* ExpandingHeap::VAllocate(u32, int) at 0x0203c6bc -- Heap vtable slot 3. Four
 * instructions: load the allocator from this+0x14 and tail-call it, passing
 * r1/r2 through untouched.
 *
 * This override always had the right spelling -- `Eji', (u32, int). SolidHeap's
 * claimed `Ejj' for the same slot, and the ROM agreed with this one; see
 * include/SolidHeap.h for the signed `blt' that settles it. */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

void* ExpandingHeap::VAllocate(u32 size, int align)
{
    return allocator->Allocate(size, align);
}
