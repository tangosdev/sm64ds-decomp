#include "types.h"
struct Heap;
struct ExpandingHeapAllocator;

struct ExpandingHeap {
    void *vtable;              /* 0x0 */
    void *heapStart;           /* 0x4 */
    u32 heapSize;              /* 0x8 */
    struct Heap *parentHeap;   /* 0xc */
    u32 flags;                 /* 0x10 */
    struct ExpandingHeapAllocator *allocator; /* 0x14 */
};

extern void *_ZTV13ExpandingHeap;
extern void _ZN4HeapC1EPvjPS_(struct ExpandingHeap *heap, void *start, u32 size, struct Heap *root);

struct ExpandingHeap *_ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator(
    struct ExpandingHeap *heap,
    void *start,
    u32 size,
    struct Heap *root,
    struct ExpandingHeapAllocator *allocator)
{
    _ZN4HeapC1EPvjPS_(heap, start, size, root);
    heap->vtable = &_ZTV13ExpandingHeap;
    heap->allocator = allocator;
    return heap;
}
