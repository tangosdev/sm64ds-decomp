//cpp
// @symbol _ZN18NestedHeapIteratorC1Ej
#include "types.h"

struct NestedHeapIterator {
    void *mHead;                /* 0x00 */
    void *mTail;                /* 0x04 */
    u16 mCount;                 /* 0x08 */
    u16 mLinkOffset;            /* 0x0a */
    NestedHeapIterator(u32 linkOffset);
};

NestedHeapIterator::NestedHeapIterator(u32 linkOffset)
{
    mHead = 0;
    mTail = 0;
    mCount = 0;
    mLinkOffset = (u16)linkOffset;
}
