#include "types.h"
// Memory::Allocate(u32 size, s32 align, Heap* heap)
// Address: 0x0203c210
// If heap is NULL, uses Memory::defaultHeapPtr. Then calls Heap::Allocate.
struct Heap {
    void* heapStart;  // 0x00
    u32   heapSize;   // 0x04
    void* parentHeap; // 0x08
    u32   flags;      // 0x0c
};

extern void* _ZN4Heap8AllocateEji(struct Heap* self, u32 size, s32 align);
extern struct Heap* _ZN6Memory14defaultHeapPtrE;

void* _ZN6Memory8AllocateEjiP4Heap(u32 size, s32 align, struct Heap* heap) {
    if (!heap) {
        heap = _ZN6Memory14defaultHeapPtrE;
    }
    return _ZN4Heap8AllocateEji(heap, size, align);
}