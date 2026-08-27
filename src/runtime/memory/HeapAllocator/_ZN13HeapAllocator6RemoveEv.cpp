//cpp
// @symbol _ZN13HeapAllocator6RemoveEv
/* recovered: named members + shared header, real C++ method */
#include "HeapAllocator.h"
// _ZN13HeapAllocator6RemoveEv
// HeapAllocator::Remove() - find containing iterator and remove self from it

typedef struct HeapAllocator HeapAllocator;
typedef struct NestedHeapIteratorT NestedHeapIteratorT;

struct NestedHeapIteratorT {
    HeapAllocator* first;
    int unk4;
    unsigned short count;
    unsigned short linkOffset;
};

extern "C" {
extern NestedHeapIteratorT* _ZN18NestedHeapIterator10FindNestedEPv(HeapAllocator* node);
extern void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(NestedHeapIteratorT* iter, HeapAllocator* node);
}

void HeapAllocator::Remove()
{
    NestedHeapIteratorT* iter = _ZN18NestedHeapIterator10FindNestedEPv(this);
    _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(iter, this);
}
