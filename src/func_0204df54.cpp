//cpp
// @symbol func_0204df54
/* Shared allocator initializer. This is not a native constructor: its final
 * AddLast call leaves the iterator count in r0, whereas mwccarm restores `this'
 * for a constructor whose body changes r0. The two callers also pass the final
 * argument as u32 while this body reads it as u16, so the ABI seam is explicit. */
#include "HeapAllocator.h"
#include "decl_NestedHeapIterator.h"

extern int data_020a4d34;
extern NestedHeapIterator data_020a4d38;

extern "C" void func_0204df54(HeapAllocator* self, u32 magic, void* start,
                               void* end, u16 flags)
{
    self->mMagic = magic;
    self->mStart = start;
    self->mEnd = end;
    self->mFlags = 0;
    self->mFlags &= ~0xffu;
    self->mFlags |= flags & 0xffu;

    _ZN18NestedHeapIteratorC1Ej(&self->mChildren, 4);

    if (data_020a4d34 == 0) {
        _ZN18NestedHeapIteratorC1Ej(&data_020a4d38, 4);
        data_020a4d34 = 1;
    }

    NestedHeapIterator::FindNested(self)->AddLast(self);
}
