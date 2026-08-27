#include "types.h"
struct SolidHeapWithIter {
    void* allocator;       // 0x00
    void* nhi_first;       // 0x04
    void* nhi_last;        // 0x08
    u16 id;                // 0x0c
    u16 nhi_offset;        // 0x0e
};

extern s32 _ZN18SolidHeapAllocator9SaveStateEj(void* alloc, u32 id);
extern s32 _ZN18SolidHeapAllocator9LoadStateEj(void* alloc, u32 id);
extern s32 func_02050fd4(struct SolidHeapWithIter* self);

s32 func_0205117c(struct SolidHeapWithIter* self)
{
    u16 id = self->id;
    void* alloc = self->allocator;
    if (!_ZN18SolidHeapAllocator9SaveStateEj(alloc, (u32)id)) {
        return ~0;
    }
    if (func_02050fd4(self)) {
        return (s32)((u32)self->id - 1);
    }
    _ZN18SolidHeapAllocator9LoadStateEj(self->allocator, 0);
    return ~0;
}