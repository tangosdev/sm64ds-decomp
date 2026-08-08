//cpp
// @symbol _ZN9SolidHeap11VDeallocateEPv
/* SolidHeap::VDeallocate(void*) at 0x0203c4e4 -- Heap vtable slot 4.
 *
 * Freeing a single block from a linear allocator is not supported, so a
 * non-null pointer is a programming error and crashes. Freeing NULL is
 * tolerated silently, which is what lets callers deallocate unconditionally.
 * VDeallocateAll is the only real free a solid heap has. */
#include "decl_common.h"
#include "SolidHeap.h"

void SolidHeap::VDeallocate(void* ptr)
{
    if (ptr == 0)
        return;
    Crash();
}
