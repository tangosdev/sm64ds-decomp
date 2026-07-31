// @symbol _ZN4HeapC1EPvjP4Heap
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Heap.h"
typedef unsigned int u32;
struct Heap;
struct HeapS {
    void *vtable;
    void *heapStart;
    u32 heapSize;
    struct Heap *parentHeap;
    u32 flags;
};
void _ZN4HeapC1EPvjP4Heap(struct HeapS *heap, void *start, u32 size, struct Heap *root) {
  heap->vtable = &data_02099d90;
  heap->heapStart = start;
  heap->heapSize = size;
  heap->parentHeap = root;
  heap->flags = 0;
  heap->flags = 0x4000;
}
