#include "types.h"
// @symbol _ZN18SolidHeapAllocator17AllocateBackwardsEPvjj
/* recovered: named members + shared header */
#include "SolidHeapAllocator.h"
extern void MultiStore_Int(s32 val, void* dst, u32 count);

void* _ZN18SolidHeapAllocator17AllocateBackwardsEPvjj(void* freeBlockPair, u32 size, u32 align)
{
    void** pair = (void**)freeBlockPair;
    void* freeBlockEnd = pair[1];
    void* freeBlockBegin = pair[0];
    void* start = (void*)(((u32)freeBlockEnd - size) & ~(align - 1));
    if (start < freeBlockBegin) {
        return (void*)0;
    }
    u32 flags = *(u32*)((char*)freeBlockPair - 4);
    u32 fillSize = (u32)freeBlockEnd - (u32)start;
    if ((u16)(flags & 0xff) & 1) {
        volatile s32 zero = 0;
        MultiStore_Int(zero, start, fillSize);
    }
    pair[1] = start;
    return start;
}
