//cpp
// @symbol _ZN9SolidHeapC1EPvjP4HeapP18SolidHeapAllocator
#include "SolidHeap.h"

SolidHeap::SolidHeap(void* start, u32 size, Heap* root,
                     SolidHeapAllocator* allocator_)
    : Heap(start, size, root), allocator(allocator_)
{
}
