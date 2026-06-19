typedef unsigned int u32;
struct Heap;
struct HeapS {
    void *vtable;
    void *heapStart;
    u32 heapSize;
    struct Heap *parentHeap;
    u32 flags;
};
extern void *data_02099d90;
void _ZN4HeapC1EPvjP4Heap(struct HeapS *heap, void *start, u32 size, struct Heap *root) {
  heap->vtable = &data_02099d90;
  heap->heapStart = start;
  heap->heapSize = size;
  heap->parentHeap = root;
  heap->flags = 0;
  heap->flags = 0x4000;
}
