typedef unsigned int u32;
struct Heap;
struct SolidHeapAllocator;
struct HeapS {
    void *vtable;
    void *heapStart;
    void *heapEnd;
    struct Heap *parentHeap;
    u32 flags;
    struct SolidHeapAllocator *allocator;
};
extern struct HeapS *_ZN6Memory14defaultHeapPtrE;
extern void *_ZN4Heap8AllocateEji(struct HeapS *self, u32 size, int align);
extern struct SolidHeapAllocator *_ZN4Heap24CreateSolidHeapAllocatorEPvjj(void *start, u32 size, u32 nodeID);
extern void _ZN4Heap10DeallocateEPv(struct HeapS *self, void *p);
extern void _ZN9SolidHeapC1EPvjP4HeapP18SolidHeapAllocator(struct HeapS *heap, void *start, u32 size, struct Heap *root, struct SolidHeapAllocator *allocator);

struct HeapS *_ZN4Heap15CreateSolidHeapEjPS_i(u32 size, struct Heap *root, int align) {
  struct HeapS *rh = (struct HeapS *)root;
  if (root == 0) rh = _ZN6Memory14defaultHeapPtrE;
  struct HeapS *heap = (struct HeapS *)_ZN4Heap8AllocateEji(rh, size + 0x18, align);
  if (heap != 0) {
    heap->heapStart = (char*)heap + 0x18;
    heap->allocator = _ZN4Heap24CreateSolidHeapAllocatorEPvjj(heap->heapStart, size, 3);
    if (heap->allocator == 0) {
      _ZN4Heap10DeallocateEPv(rh, heap);
      heap = 0;
    } else {
      heap->heapEnd = (void*)size;
      if (heap != 0) {
        _ZN9SolidHeapC1EPvjP4HeapP18SolidHeapAllocator(heap, heap->heapStart, (u32)heap->heapEnd, (struct Heap*)rh, heap->allocator);
      }
    }
  }
  return heap;
}
