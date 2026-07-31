#include "types.h"
extern void _ZN18SolidHeapAllocator10ResetStartEv(void* self);
extern void _ZN18SolidHeapAllocator8ResetEndEv(void* self);

// SolidHeapAllocator::Reset(u32 params)
// bit 0: call ResetStart; bit 1: call ResetEnd
void _ZN18SolidHeapAllocator5ResetEj(void* self, u32 params)
{
    if (params & 1) {
        _ZN18SolidHeapAllocator10ResetStartEv(self);
    }
    if (params & 2) {
        _ZN18SolidHeapAllocator8ResetEndEv(self);
    }
}