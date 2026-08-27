#include "types.h"
struct SolidHeapWithIter {
    void* allocator;       // 0x00
    void* nhi_first;       // 0x04
    void* nhi_last;        // 0x08
    u16 id;                // 0x0c
    u16 nhi_offset;        // 0x0e
};

extern void _ZN18NestedHeapIteratorC1Ej(void* self, u32 offset);
extern s32 func_02050fd4(struct SolidHeapWithIter* self);

s32 func_02051024(struct SolidHeapWithIter* self, void* allocator)
{
    _ZN18NestedHeapIteratorC1Ej(&self->nhi_first, 0xc);
    self->allocator = allocator;
    if (func_02050fd4(self)) {
        return 1;
    }
    return 0;
}