//cpp
// @symbol _ZN4Heap14CreateRootHeapEPvj
/* Heap::CreateRootHeap(void*, u32) at 0x0203c8e8 -- build the one heap that has
 * no parent, directly on a block the caller already owns.
 *
 * Unlike CreateSolidHeap and CreateExpandingHeap this allocates nothing: it is
 * handed raw memory by Heap::SetupRootHeap, which took it from the OS arena, and
 * lays an ExpandingHeap over the front of it. So the 0x18 appears twice for the
 * same reason as in those two -- it is the size of the derived object -- but
 * here it is subtracted from what the caller gave rather than added to what is
 * requested. `parentHeap' comes out NULL, which is what makes this the root.
 *
 * THIS FUNCTION IS WHY include/Heap.h HAS NO FIELD AT 0x14. It writes one, and
 * it is static, so the evidence pass attributed the write to Heap through a
 * pointer that is really an ExpandingHeap under construction -- a single witness
 * against the two derived constructors, which both write 0x14 only after calling
 * Heap's constructor. That is now expressible: the object is an ExpandingHeap*
 * here, and the field belongs to it.
 *
 * The shadow this replaces typed the second parameter `void* end' and called
 * 0x08 `heapEnd'. The mangled name says `j' -- a u32 -- and both values are
 * sizes. */
#include "Heap.h"
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

extern "C" ExpandingHeap* _ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator(
    ExpandingHeap* heap, void* start, u32 size, Heap* root, ExpandingHeapAllocator* allocator);

Heap* Heap::CreateRootHeap(void* mem, u32 size)
{
    ExpandingHeap* heap = (ExpandingHeap*)mem;

    heap->heapStart = (char*)heap + 0x18;
    heap->allocator = CreateExpandingHeapAllocator(heap->heapStart, size - 0x18, 3);
    if (heap->allocator != 0)
    {
        heap->heapSize = size - 0x18;
        if (heap != 0)
            _ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator(
                heap, heap->heapStart, heap->heapSize, 0, heap->allocator);
        return heap;
    }

    heap->heapSize = 0;
    return 0;
}
