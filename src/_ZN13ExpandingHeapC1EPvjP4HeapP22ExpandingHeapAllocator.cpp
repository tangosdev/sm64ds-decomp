//cpp
// @symbol _ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator
#include "ExpandingHeap.h"

ExpandingHeap::ExpandingHeap(void* start, u32 size, Heap* root,
                             ExpandingHeapAllocator* allocator_)
    : Heap(start, size, root), allocator(allocator_)
{
}
