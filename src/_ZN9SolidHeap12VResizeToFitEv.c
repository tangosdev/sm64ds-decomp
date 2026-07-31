#include "types.h"
/* SolidHeap::VResizeToFit at 0x0203c33c */
struct Heap {
    void* vtable;
    void* heapStart;
    u32 heapSize;
    struct Heap* parentHeap;
    u32 flags;
};

struct SolidHeapAllocator;

struct SolidHeap {
    struct Heap base;
    struct SolidHeapAllocator* allocator;
};

extern u32 _ZN18SolidHeapAllocator14TryResizeToFitEv(struct SolidHeapAllocator* alloc);
extern u32 _ZN4Heap10ReallocateEPvj(struct Heap* heap, void* ptr, u32 newSize);

u32 _ZN9SolidHeap12VResizeToFitEv(struct SolidHeap* thiz)
{
    u32 r4 = _ZN18SolidHeapAllocator14TryResizeToFitEv(thiz->allocator);
    if (!r4)
        return 0;
    u32 r5 = r4 + 0x18;
    if (!_ZN4Heap10ReallocateEPvj(thiz->base.parentHeap, thiz, r5))
        return 0;
    thiz->base.heapSize = r4;
    return r5;
}
