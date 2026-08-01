#include "types.h"
struct Heap;
struct SolidHeapAllocator;

struct SolidHeap {
    void *vtable;              /* 0x0 */
    void *heapStart;           /* 0x4 */
    u32 heapSize;              /* 0x8 */
    struct Heap *parentHeap;   /* 0xc */
    u32 flags;                 /* 0x10 */
    struct SolidHeapAllocator *allocator; /* 0x14 */
};

extern void *_ZTV9SolidHeap;
extern void _ZN4HeapC1EPvjP4Heap(struct SolidHeap *heap, void *start, u32 size, struct Heap *root);

struct SolidHeap *_ZN9SolidHeapC1EPvjP4HeapP18SolidHeapAllocator(
    struct SolidHeap *heap,
    void *start,
    u32 size,
    struct Heap *root,
    struct SolidHeapAllocator *allocator)
{
    _ZN4HeapC1EPvjP4Heap(heap, start, size, root);
    heap->vtable = &_ZTV9SolidHeap;
    heap->allocator = allocator;
    return heap;
}
