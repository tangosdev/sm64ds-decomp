//cpp
// @symbol _ZN18NestedHeapIteratorC1Ej
#include "NestedHeapIterator.h"

NestedHeapIterator::NestedHeapIterator(u32 linkOffset)
    : mFirst(0), mLast(0), mCount(0), mLinkOffset(linkOffset)
{
}
