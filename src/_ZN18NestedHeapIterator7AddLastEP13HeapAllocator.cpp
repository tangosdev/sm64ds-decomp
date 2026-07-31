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
        int last = mLast;
        *(int *)(a + link_off) = last;
        *(int *)(a + link_off + 4) = 0;
        link_off = mLinkOffset;
        last = mLast;
        *(int *)((char *)last + link_off + 4) = (int)a;
        mLast = (int)a;
        *(unsigned short *)(int)(((long long)(int)((char *)&unk_008))) += 1;
    }
}
