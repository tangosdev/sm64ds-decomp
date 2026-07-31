#include "types.h"
// @symbol _ZN18SolidHeapAllocator16AllocateForwardsEPvjj
/* recovered: named members + shared header */
#include "SolidHeapAllocator.h"
extern void MultiStore_Int(s32 val, void* dst, u32 count);

void* _ZN18SolidHeapAllocator16AllocateForwardsEPvjj(void* freeBlockPair, u32 size, u32 align)
{
    void** pair = (void**)freeBlockPair;
    void* freeBlockBegin = pair[0];
    void* start = (void*)(((align - 1) + (u32)freeBlockBegin) & ~(align - 1));
    void* freeBlockEnd = pair[1];
    void* end = (void*)((u32)size + (u32)start);
    if (end > freeBlockEnd) {
        return (void*)0;
    }
    u32 flags = *(u32*)((char*)freeBlockPair - 4);
    u32 fillSize = (u32)end - (u32)freeBlockBegin;
    if ((u16)(flags & 0xff) & 1) {
        volatile s32 zero = 0;
        MultiStore_Int(zero, freeBlockBegin, fillSize);
    }
    pair[0] = end;
    return start;
}
