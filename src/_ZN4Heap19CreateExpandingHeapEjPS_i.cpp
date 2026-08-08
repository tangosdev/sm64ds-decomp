//cpp
// @symbol _ZN4Heap19CreateExpandingHeapEjPS_i
/* Heap::CreateExpandingHeap(u32, Heap*, int) at 0x0203c844 -- the expanding twin
 * of Heap::CreateSolidHeap, instruction for instruction bar the allocator it
 * builds and the constructor it calls. See that file for the 0x18 and for why
 * neither could be migrated until ExpandingHeap became a real type.
 *
 * Same correction: 0x08 is `heapSize' and always held a size, not an end
 * address. */
#include "Heap.h"
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

namespace Memory { extern Heap* defaultHeapPtr; }   /* 0x020a0ea0 */

extern "C" ExpandingHeap* _ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator(
    ExpandingHeap* heap, void* start, u32 size, Heap* root, ExpandingHeapAllocator* allocator);

Heap* Heap::CreateExpandingHeap(u32 size, Heap* root, int align)
{
    Heap* parent = root;
    if (root == 0)
        parent = Memory::defaultHeapPtr;

    ExpandingHeap* heap = (ExpandingHeap*)parent->Allocate(size + 0x18, align);
    if (heap != 0)
    {
        heap->heapStart = (char*)heap + 0x18;
        heap->allocator = CreateExpandingHeapAllocator(heap->heapStart, size, 3);
        if (heap->allocator == 0)
        {
            parent->Deallocate(heap);
            heap = 0;
        }
        else
        {
            heap->heapSize = size;
            if (heap != 0)
                _ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator(
                    heap, heap->heapStart, heap->heapSize, parent, heap->allocator);
        }
    }
    return heap;
}
