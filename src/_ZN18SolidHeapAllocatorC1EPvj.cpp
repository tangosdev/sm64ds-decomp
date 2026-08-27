//cpp
// @symbol _ZN18SolidHeapAllocatorC1EPvj
#include "SolidHeapAllocator.h"
#include "decl_HeapAllocator.h"

SolidHeapAllocator::SolidHeapAllocator(void* heapEnd, u32 flags)
{
    SolidHeapFreeRegion* freeRegion = &mFreeRegion;

    func_0204df54(this, 0x46524d48, (char*)freeRegion + 0xc,
                  heapEnd, flags);
    freeRegion->begin = mStart;
    freeRegion->end = mEnd;
    freeRegion->flags = 0;
}
