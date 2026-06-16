// ExpandingHeap::VDestroy()
// Address: 0x0203c72c
// Destroys the allocator and sets allocator ptr to NULL

typedef unsigned int u32;

struct HeapAllocator;

extern void _ZN13HeapAllocator7DestroyEv(struct HeapAllocator* self);

struct Heap {
    void* vtable;       // 0x00
    void* heapStart;    // 0x04
    u32   heapSize;     // 0x08
    void* parentHeap;   // 0x0c
    u32   flags;        // 0x10
};

struct ExpandingHeap {
    struct Heap base;                       // 0x00..0x13
    struct HeapAllocator* allocator;        // 0x14
};

void _ZN9SolidHeap8VDestroyEv(struct ExpandingHeap* self) {
    _ZN13HeapAllocator7DestroyEv(self->allocator);
    self->allocator = 0;
}
