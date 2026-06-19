typedef unsigned int u32;
struct Heap;
struct ExpandingHeapAllocator;
struct HeapS {
    void *vtable;              /* 0x0 */
    void *heapStart;           /* 0x4 */
    void *heapEnd;             /* 0x8 */
    struct Heap *parentHeap;   /* 0xc */
    u32 flags;                 /* 0x10 */
    struct ExpandingHeapAllocator *allocator; /* 0x14 */
};
extern struct ExpandingHeapAllocator *_ZN4Heap28CreateExpandingHeapAllocatorEPvjj(void *start, u32 size, u32 nodeID);
extern void _ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator(struct HeapS *heap, void *start, u32 size, struct Heap *root, struct ExpandingHeapAllocator *allocator);

struct HeapS *_ZN4Heap14CreateRootHeapEPvj(struct HeapS *heap, void *end) {
  heap->heapStart = (char*)heap + 0x18;
  heap->allocator = _ZN4Heap28CreateExpandingHeapAllocatorEPvjj(heap->heapStart, (u32)((char*)end - 0x18), 3);
  if (heap->allocator != 0) {
    heap->heapEnd = (char*)end - 0x18;
    if (heap != 0) {
      _ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator(heap, heap->heapStart, (u32)heap->heapEnd, (struct Heap*)0, heap->allocator);
    }
    return heap;
  }
  heap->heapEnd = 0;
  return (struct HeapS*)0;
}
