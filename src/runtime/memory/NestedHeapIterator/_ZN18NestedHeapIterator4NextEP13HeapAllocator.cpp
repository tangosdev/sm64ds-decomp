//cpp
// @symbol _ZN18NestedHeapIterator4NextEP13HeapAllocator
/* recovered: named members + shared header, real C++ method */
#include "NestedHeapIterator.h"
struct HeapAllocator;


int NestedHeapIterator::Next(HeapAllocator * h_)
{
    char* h = (char*)h_;

  if (h == 0) return *(int*)((char*)this);
  unsigned short off = mLinkOffset;
  return *(int*)(h+off+4);
}
