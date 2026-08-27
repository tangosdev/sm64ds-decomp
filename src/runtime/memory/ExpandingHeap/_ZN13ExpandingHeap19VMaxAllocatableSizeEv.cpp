//cpp
// @symbol _ZN13ExpandingHeap19VMaxAllocatableSizeEv
/* ExpandingHeap::VMaxAllocatableSize() at 0x0203c5d0 -- Heap vtable slot 11.
 * The LARGEST SINGLE free node at 4-byte alignment, not the total; see
 * VMemoryLeft for why the distinction matters here and not on a solid heap.
 *
 * Reached `*(void**)((char*)&unk_014)' through an address cast before the
 * allocator was a named, typed member. */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

u32 ExpandingHeap::VMaxAllocatableSize()
{
    return allocator->MaxAllocatableSize(4);
}
