#include "types.h"
// ExpandingHeap::VDeallocate(void* ptr)
// Address: 0x0203c50c
struct ExpandingHeapAllocator;

extern void _ZN22ExpandingHeapAllocator10DeallocateEPv(struct ExpandingHeapAllocator* self, void* ptr);

struct Heap {
    void* vtable;       // 0x00
    void* heapStart;    // 0x04
    u32   heapSize;     // 0x08
    void* parentHeap;   // 0x0c
    u32   flags;        // 0x10
};

struct ExpandingHeap {
    struct Heap base;                       // 0x00..0x13
    struct ExpandingHeapAllocator* allocator; // 0x14
};

void _ZN13ExpandingHeap11VDeallocateEPv(struct ExpandingHeap* self, void* ptr) {
    if (!ptr) {
        return;
    }
    _ZN22ExpandingHeapAllocator10DeallocateEPv(self->allocator, ptr);
}