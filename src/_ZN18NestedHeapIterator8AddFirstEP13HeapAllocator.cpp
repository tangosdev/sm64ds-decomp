//cpp
// @symbol _ZN18NestedHeapIterator8AddFirstEP13HeapAllocator
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_NestedHeapIterator.h"
/* recovered: named members + shared header, real C++ method */
#include "NestedHeapIterator.h"
struct HeapAllocator;
#define AT(p, off) ((void *)(int)(((long long)(int)((char *)(p) + (off)))))
struct HeapAllocator;

void NestedHeapIterator::AddFirst(HeapAllocator * a_)
{
    char* a = (char*)a_;

    if (*(char**)((char*)this) == 0) { _ZN18NestedHeapIterator4InitEP13HeapAllocator(((char*)this), a); return; }
    {
        int* pa = (int*)(a + mLinkOffset);
        pa[0] = 0;
        pa[1] = *(int*)((char*)this);
    }
    *(int*)(*(char**)((char*)this) + mLinkOffset) = (int)a;
    *(char**)((char*)this) = a;
    {
        unsigned short* cnt = (unsigned short*)AT(((char*)this), 8);
        *cnt = *cnt + 1;
    }
}
