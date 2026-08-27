//cpp
// @symbol _ZN9SolidHeap19VMaxAllocatableSizeEv
/* SolidHeap::VMaxAllocatableSize() at 0x0203c5bc -- Heap vtable slot 11.
 * Identical to slots 10 and 12 on a linear allocator; see VMemoryLeft. */
#include "SolidHeap.h"
#include "SolidHeapAllocator.h"

u32 SolidHeap::VMaxAllocatableSize()
{
    return allocator->MemoryLeft(4);
}
