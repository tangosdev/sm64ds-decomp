//cpp
// @symbol _ZN18NestedHeapIterator4InitEP13HeapAllocator
/* recovered: named members + shared header, real C++ method */
#include "NestedHeapIterator.h"
struct HeapAllocator;
struct HeapAllocator;

void NestedHeapIterator::Init(HeapAllocator * a_)
{
    char * a = (char *)a_;

  char *new_var = a + (*((unsigned short *) ((char *)&mLinkOffset)));
  *((int *) ((a + (*((unsigned short *) ((char *)&mLinkOffset)))) + 4)) = 0;
  *((int *) new_var) = 0;
  *((int *) ((char *)this)) = (int) a;
  *((int *) ((char *)&mLast)) = (int) a;
  *(unsigned short *)(((long long)(int)((char *)&unk_008))) += 1;
}
