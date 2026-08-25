#include "types.h"
// @symbol _ZN22ExpandingHeapAllocatorC1EPvj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_HeapAllocator.h"
/* recovered: named members + shared header */
#include "ExpandingHeapAllocator.h"
// ExpandingHeapAllocator constructor: runs the HeapAllocator base ctor with
// magic 'HPXE', zeroes the two u16 counters at +0x34/+0x36, clears bit 0 of
// the +0x36 flags, creates the initial free node ('FR') spanning the heap
// range at +0x18/+0x1c, and initializes the node list head/tail at +0x24.
typedef struct {
    void *start;
    void *end;
} Target;

typedef struct MemoryNode MemoryNode;

extern MemoryNode *_ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(Target *t, u16 v);

void *_ZN22ExpandingHeapAllocatorC1EPvj(struct ExpandingHeapAllocator *self, void *start, u32 size) {
    char *inner = ((char *)self) + 0x24;
    Target t;
    MemoryNode *node;
    _ZN13HeapAllocatorC1EjPvPvj(((char *)self), 0x45585048, inner + 0x14, start, size);
    *(u16 *)(inner + 0x10) = 0;
    *(u16 *)(inner + 0x12) = 0;
    (*(u16 *)(inner + 0x12)) &= ~1;
    t.start = *(void **)((char *)&self->mStart);
    t.end = *(void **)((char *)&self->mEnd);
    node = _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(&t, 0x4652);
    *(MemoryNode **)(inner) = node;
    *(MemoryNode **)(inner + 4) = node;
    *(u32 *)(inner + 8) = 0;
    *(u32 *)(inner + 0xc) = 0;
    return ((char *)self);
}
