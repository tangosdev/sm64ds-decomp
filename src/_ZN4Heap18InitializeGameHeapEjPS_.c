#include "types.h"
// Heap::InitializeGameHeap(u32 size, Heap* root)
// Address: 0x0203c24c
// Creates an ExpandingHeap and stores it to Memory::gameHeapPtr
struct Heap {
    void* heapStart;
    u32   heapSize;
    void* parentHeap;
    u32   flags;
};

struct ExpandingHeap {
    struct Heap base;
    void* allocator;
};

extern struct ExpandingHeap* _ZN4Heap19CreateExpandingHeapEjPS_i(u32 size, struct Heap* root, s32 align);
extern struct ExpandingHeap* data_020a0eac;

void _ZN4Heap18InitializeGameHeapEjPS_(u32 size, struct Heap* root) {
    data_020a0eac = _ZN4Heap19CreateExpandingHeapEjPS_i(size, root, 4);
}