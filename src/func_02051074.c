#include "types.h"
typedef struct SolidHeapAllocator SolidHeapAllocator;
extern u32 _ZN18SolidHeapAllocator10MemoryLeftEi(SolidHeapAllocator *self, u32 align);
u32 func_02051074(SolidHeapAllocator **pHeap)
{
    u32 mem;
    mem = _ZN18SolidHeapAllocator10MemoryLeftEi(*pHeap, 0x20);
    if (mem < 0x20)
        return 0;
    mem -= 0x20;
    mem &= ~0x1fu;
    return mem;
}
