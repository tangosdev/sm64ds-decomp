//cpp
// @symbol _ZN18NestedHeapIterator10FindNestedEPv
/* NestedHeapIterator::FindNested -- the public entry point over
 * RecursiveFindNested: start from the file-scope root iterator and hand back the
 * innermost iterator that owns `addr', or the root itself when nothing does.
 *
 * STATIC: the body takes the pointer in r0 and reads no second register, and it
 * gets its starting iterator from a global rather than from an object.
 *
 * RecursiveFindNested returns the owning HeapAllocator, whose own nested
 * iterator is the sub-object at +0xc -- the same offset that function recurses
 * through, and for the same reason left as a raw cast: nothing matched has
 * pinned that sub-object from the allocator side, so include/HeapAllocator.h
 * does not type it.
 */
#include "NestedHeapIterator.h"

extern "C" NestedHeapIterator data_020a4d38;

NestedHeapIterator *NestedHeapIterator::FindNested(void *addr)
{
    NestedHeapIterator *root = &data_020a4d38;
    void *found = root->RecursiveFindNested(addr);
    if (found) {
        return (NestedHeapIterator *)((char *)found + 0xc);
    }
    return root;
}
