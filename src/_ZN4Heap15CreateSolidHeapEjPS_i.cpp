//cpp
// @symbol _ZN4Heap15CreateSolidHeapEjPS_i
/* Heap::CreateSolidHeap(u32, Heap*, int) at 0x0203c7a0 -- allocate a block out
 * of `root' and build a SolidHeap in the first 0x18 bytes of it, with the
 * remaining `size' bytes as the new heap's arena.
 *
 * The 0x18 is the derived object: Heap is 0x14 and SolidHeap adds `allocator' at
 * 0x14, so the header sits in [heap, heap+0x18) and heapStart is the first byte
 * past it. That is why the request is `size + 0x18' but the arena handed to the
 * allocator is `size'.
 *
 * A NULL `root' means the current default heap.
 *
 * This could not be migrated with the rest of Heap's methods: it writes
 * heapStart, heapSize and allocator on a raw block BEFORE the constructor runs,
 * so it needs SolidHeap to be a real type with real members. Until this slice,
 * include/SolidHeap.h modelled the base as `u8 pad_000[0x14]' and this file
 * carried a local shadow instead -- one that called 0x08 `heapEnd', stored
 * `(void*)size' into it, then read it back and cast it to u32 to pass on. The
 * field is `heapSize' and the value was a size the whole way through.
 *
 * The `if (heap != 0)' before the constructor is redundant -- the enclosing
 * branch already established it -- but the ROM emits the second test, so it
 * stays. */
#include "Heap.h"
#include "SolidHeap.h"
#include "SolidHeapAllocator.h"

namespace Memory { extern Heap* defaultHeapPtr; }   /* 0x020a0ea0 */

extern "C" SolidHeap* _ZN9SolidHeapC1EPvjP4HeapP18SolidHeapAllocator(
    SolidHeap* heap, void* start, u32 size, Heap* root, SolidHeapAllocator* allocator);

Heap* Heap::CreateSolidHeap(u32 size, Heap* root, int align)
{
    Heap* parent = root;
    if (root == 0)
        parent = Memory::defaultHeapPtr;

    SolidHeap* heap = (SolidHeap*)parent->Allocate(size + 0x18, align);
    if (heap != 0)
    {
        heap->heapStart = (char*)heap + 0x18;
        heap->allocator = CreateSolidHeapAllocator(heap->heapStart, size, 3);
        if (heap->allocator == 0)
        {
            parent->Deallocate(heap);
            heap = 0;
        }
        else
        {
            heap->heapSize = size;
            if (heap != 0)
                _ZN9SolidHeapC1EPvjP4HeapP18SolidHeapAllocator(
                    heap, heap->heapStart, heap->heapSize, parent, heap->allocator);
        }
    }
    return heap;
}
