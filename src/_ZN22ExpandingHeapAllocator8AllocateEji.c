#include "types.h"
extern void* _ZN22ExpandingHeapAllocator17AllocateBackwardsEjj(void* self, u32 size, u32 align);
extern void* _ZN22ExpandingHeapAllocator16AllocateForwardsEjj(void* self, u32 size, u32 align);

// ExpandingHeapAllocator::Allocate(u32 size, s32 align)
// align < 0 = backwards, >= 0 = forwards
void* _ZN22ExpandingHeapAllocator8AllocateEji(void* self, u32 size, s32 align)
{
    if (size == 0) size = 1;
    size = (size + 3) & ~3u;
    if (align >= 0) {
        return _ZN22ExpandingHeapAllocator16AllocateForwardsEjj(self, size, (u32)align);
    }
    return _ZN22ExpandingHeapAllocator17AllocateBackwardsEjj(self, size, (u32)-align);
}