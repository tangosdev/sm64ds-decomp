// @symbol _ZN4Heap24CreateSolidHeapAllocatorEPvjj
/* recovered: named members + shared header */
#include "Heap.h"
// Heap::CreateSolidHeapAllocator(void* address, u32 size, u32 flags)
// Address: 0x0204ebc4
// Try: compute avail only in second condition to defer sub

typedef unsigned int u32;

struct SolidHeapAllocator;

extern struct SolidHeapAllocator* _ZN18SolidHeapAllocatorC1EPvj(
    struct SolidHeapAllocator* self, void* heapEnd, u32 flags);

struct SolidHeapAllocator* _ZN4Heap24CreateSolidHeapAllocatorEPvjj(
    void* address, u32 size, u32 flags)
{
    u32 end_u   = (size + (u32)address) & ~3u;
    u32 start_u = ((u32)address + 3u)   & ~3u;
    if (start_u > end_u || (end_u - start_u) < 0x30) {
        return 0;
    }
    return _ZN18SolidHeapAllocatorC1EPvj(
        (struct SolidHeapAllocator*)start_u, (void*)end_u, flags);
}