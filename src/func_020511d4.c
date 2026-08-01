#include "types.h"
typedef struct HeapAllocator HeapAllocator;

typedef struct NestedHeapIterator {
    int unk[8];
} NestedHeapIterator;

typedef struct HeapOwner {
    HeapAllocator     *heap;
    NestedHeapIterator iter;
} HeapOwner;

typedef struct AllocNode {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkc;
    u32 unk10;
    u32 unk14;
} AllocNode;

extern void *_ZN18SolidHeapAllocator8AllocateEji(HeapAllocator *heap, u32 size, s32 align);
extern NestedHeapIterator *_ZN18NestedHeapIterator8PreviousEP13HeapAllocator(NestedHeapIterator *iter, HeapAllocator *heap);
extern void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(NestedHeapIterator *iter, HeapAllocator *heap);

void *func_020511d4(HeapOwner *owner, u32 elemSize, u32 param2, u32 param3, u32 param4)
{
    AllocNode *node;
    NestedHeapIterator *iter;

    node = (AllocNode *)_ZN18SolidHeapAllocator8AllocateEji(
        owner->heap,
        ((elemSize + 0x1f) & ~0x1fu) + 0x20,
        0x20);
    if (node == 0)
        return 0;

    iter = _ZN18NestedHeapIterator8PreviousEP13HeapAllocator(&owner->iter, 0);
    node->unk8  = elemSize;
    node->unkc  = param2;
    node->unk10 = param3;
    node->unk14 = param4;
    _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(iter, (HeapAllocator *)node);

    return (void *)((u32)node + 0x20);
}
