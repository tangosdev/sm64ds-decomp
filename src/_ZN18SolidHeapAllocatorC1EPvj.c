// @symbol _ZN18SolidHeapAllocatorC1EPvj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_HeapAllocator.h"
/* recovered: named members + shared header */
#include "SolidHeapAllocator.h"
typedef unsigned int u32;

struct FreeList { void* begin; void* end; int flags; };


void* _ZN18SolidHeapAllocatorC1EPvj(struct SolidHeapAllocator *self, void* ptr, u32 size) {
    struct FreeList *fl;

    fl = (struct FreeList *)((char *)&self->mFreeRegion);
    _ZN13HeapAllocatorC1EjPvPvj(((void*)self), 0x46524d48, (char *)fl + 0xc, ptr, size);
    *(void **)(((long long)(int)fl)) =
        *(void **)((char *)&self->unk_018);
    fl->end = *(void **)((char *)&self->mEnd);
    fl->flags = 0;
    return ((void*)self);
}