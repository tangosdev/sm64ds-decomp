typedef unsigned int u32;
struct Heap;
struct ExpandingHeapAllocator;
struct HeapS {
    void *vtable;
    void *heapStart;
    void *heapEnd;
    struct Heap *parentHeap;
    u32 flags;
    struct ExpandingHeapAllocator *allocator;
};
extern struct HeapS *data_020a0ea0;
extern void *_ZN4Heap8AllocateEji(struct HeapS *self, u32 size, int align);
extern struct ExpandingHeapAllocator *_ZN4Heap28CreateExpandingHeapAllocatorEPvjj(void *start, u32 size, u32 nodeID);
extern void _ZN4Heap10DeallocateEPv(struct HeapS *self, void *p);
extern void _ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator(struct HeapS *heap, void *start, u32 size, struct Heap *root, struct ExpandingHeapAllocator *allocator);

struct HeapS *_ZN4Heap19CreateExpandingHeapEjPS_i(u32 size, struct Heap *root, int align) {
  struct HeapS *rh = (struct HeapS *)root;
  if (root == 0) rh = data_020a0ea0;
  struct HeapS *heap = (struct HeapS *)_ZN4Heap8AllocateEji(rh, size + 0x18, align);
  if (heap != 0) {
    heap->heapStart = (char*)heap + 0x18;
    heap->allocator = _ZN4Heap28CreateExpandingHeapAllocatorEPvjj(heap->heapStart, size, 3);
    if (heap->allocator == 0) {
      _ZN4Heap10DeallocateEPv(rh, heap);
      heap = 0;
    } else {
      heap->heapEnd = (void*)size;
      if (heap != 0) {
        _ZN13ExpandingHeapC1EPvjP4HeapP22ExpandingHeapAllocator(heap, heap->heapStart, (u32)heap->heapEnd, (struct Heap*)rh, heap->allocator);
      }
    }
  }
  return heap;
}
