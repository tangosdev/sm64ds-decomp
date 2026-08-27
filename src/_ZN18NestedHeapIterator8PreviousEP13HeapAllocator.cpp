//cpp
// @symbol _ZN18NestedHeapIterator8PreviousEP13HeapAllocator
/* recovered: named members + shared header, real C++ method */
#include "NestedHeapIterator.h"
struct HeapAllocator;


int NestedHeapIterator::Previous(HeapAllocator * h_)
{
    char* h = (char*)h_;

  if (h == 0) return (int)mLast;
  unsigned short off = mLinkOffset;
  return *(int*)(h+off);
}
