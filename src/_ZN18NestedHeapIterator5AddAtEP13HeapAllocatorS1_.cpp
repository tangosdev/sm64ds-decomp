//cpp
// @symbol _ZN18NestedHeapIterator5AddAtEP13HeapAllocatorS1_
/* NestedHeapIterator::AddAt -- splice `node_' into this list immediately before
 * `at_'. A null `at_' means append, and `at_' being the head means prepend, so
 * both ends delegate to the existing AddLast/AddFirst members; only the middle
 * case does the four-pointer relink itself.
 *
 * The links live at `mLinkOffset' bytes into each HeapAllocator -- prev at +0,
 * next at +4 -- which is why this walks through char* rather than through named
 * members: the offset is a runtime value, not a layout the header can spell.
 *
 * The count at mCount is incremented SIXTEEN bits wide, so the cast is real
 * type information and not a codegen crutch: the header types the field u8
 * with a byte of padding after it, and a plain `mCount += 1' misses by two
 * words (measured). What this file does NOT need is the launder the sibling
 * AddLast still carries -- `*(unsigned short *)(int)((char *)&mCount)' and
 * `*(unsigned short *)&mCount' assemble identically here. The clean
 * follow-up is to retype the field u16 across the class; that touches
 * already-matched files, so it is not this change.
 */
#include "NestedHeapIterator.h"
#include "HeapAllocator.h"

void NestedHeapIterator::AddAt(HeapAllocator *at_, HeapAllocator *node_)
{
    if (at_ == 0) {
        AddLast(node_);
        return;
    }
    if ((char *)at_ == *(char **)this) {
        AddFirst(node_);
        return;
    }
    {
        unsigned short off = mLinkOffset;
        char *nlink = (char *)node_ + off;
        char *prev = *(char **)((char *)at_ + off);
        *(char **)nlink = prev;
        *(char **)(nlink + 4) = (char *)at_;
        *(char **)(prev + off + 4) = (char *)node_;
        *(char **)((char *)at_ + mLinkOffset) = (char *)node_;
        *(unsigned short *)&mCount += 1;
    }
}
