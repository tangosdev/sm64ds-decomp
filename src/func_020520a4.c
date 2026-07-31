#include "types.h"
typedef struct HeapAllocator HeapAllocator;
typedef struct NestedHeapIterator {
    int unk[8];
} NestedHeapIterator;

extern NestedHeapIterator gNestedHeapIter;  /* 0x020a5bbc */

extern u32  _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);
extern void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(NestedHeapIterator *iter, HeapAllocator *heap);

void func_020520a4(HeapAllocator *node)
{
    u32 state;

    state = _ZN3IRQ7DisableEv();
    _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(&gNestedHeapIter, node);
    _ZN3IRQ7RestoreEj(state);
}
