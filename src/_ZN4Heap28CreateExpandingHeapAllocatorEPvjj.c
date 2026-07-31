// @symbol _ZN4Heap28CreateExpandingHeapAllocatorEPvjj
/* recovered: named members + shared header */
#include "Heap.h"
// Heap::CreateExpandingHeapAllocator(void* address, u32 size, u32 flags)
// Address: 0x0204e3c0
// Inline avail expression in || to defer sub r3,r1,r0 after bhi

typedef unsigned int u32;

struct ExpandingHeapAllocator;

extern struct ExpandingHeapAllocator* _ZN22ExpandingHeapAllocatorC1EPvj(
    struct ExpandingHeapAllocator* self, void* heapEnd, u32 flags);

struct ExpandingHeapAllocator* _ZN4Heap28CreateExpandingHeapAllocatorEPvjj(
    void* address, u32 size, u32 flags)
{
    u32 end_u   = (size + (u32)address) & ~3u;
    u32 start_u = ((u32)address + 3u)   & ~3u;
    if (start_u > end_u || (end_u - start_u) < 0x4c) {
        return 0;
    }
    return _ZN22ExpandingHeapAllocatorC1EPvj(
        (struct ExpandingHeapAllocator*)start_u, (void*)end_u, flags);
}