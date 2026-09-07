//cpp
// @symbol _ZN9SolidHeap14VDeallocateAllEv
/* SolidHeap::VDeallocateAll() at 0x0203c49c -- Heap vtable slot 5. Throws the
 * whole arena away at once, which for a linear allocator is the only kind of
 * free that works: individual blocks cannot be returned (VDeallocate calls
 * Crash), so everything goes together or nothing does. */
#include "SolidHeap.h"
#include "SolidHeapAllocator.h"

void SolidHeap::VDeallocateAll()
{
    /* 3 = both bits -- ResetStart and ResetEnd. */
    allocator->Reset(3);
}
