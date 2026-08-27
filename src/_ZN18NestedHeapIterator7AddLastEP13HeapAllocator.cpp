//cpp
// @symbol _ZN18NestedHeapIterator7AddLastEP13HeapAllocator
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_NestedHeapIterator.h"
/* recovered: named members + shared header, real C++ method */
#include "NestedHeapIterator.h"
struct HeapAllocator;

void NestedHeapIterator::AddLast(HeapAllocator * a_)
{
    char * a = (char *)a_;

    if (*(int *)((char *)this) == 0) { _ZN18NestedHeapIterator4InitEP13HeapAllocator(((char *)this), a); return; }
    {
        unsigned short link_off = mLinkOffset;
        int last = (int)mLast;
        *(int *)(a + link_off) = last;
        *(int *)(a + link_off + 4) = 0;
        link_off = mLinkOffset;
        last = (int)mLast;
        *(int *)((char *)last + link_off + 4) = (int)a;
        mLast = (HeapAllocator*)a;
        *(unsigned short *)(int)((char *)&mCount) += 1;
    }
}
