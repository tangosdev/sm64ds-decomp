//cpp
// @symbol _ZN9SolidHeap22VMaxAllocationUnitSizeEv
/* SolidHeap::VMaxAllocationUnitSize() at 0x0203c5e4 -- Heap vtable slot 10.
 * Identical to slots 11 and 12 on a linear allocator; see VMemoryLeft. */
#include "SolidHeap.h"
#include "SolidHeapAllocator.h"

u32 SolidHeap::VMaxAllocationUnitSize()
{
    return allocator->MemoryLeft(4);
}
