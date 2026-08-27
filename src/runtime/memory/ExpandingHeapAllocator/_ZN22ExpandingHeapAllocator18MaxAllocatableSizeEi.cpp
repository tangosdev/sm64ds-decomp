//cpp
// @symbol _ZN22ExpandingHeapAllocator18MaxAllocatableSizeEi
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::MaxAllocatableSize(int align) at 0x0204e0f8 -- uses `this`.
 *
 * The largest single block still obtainable at this alignment -- NOT the same as
 * MemoryLeft, which sums every free node. Walks the free list at this+0x24, aligns
 * each node's data start (node+0x10) upward, and keeps the largest usable remainder;
 * ties are broken toward the node that wastes least on alignment padding, which is
 * what `bestOffset` tracks.
 *
 * `align` goes through cstd::abs, so the sign convention Allocate uses (negative means
 * allocate from the high end) is accepted and ignored -- the answer is the same either
 * way. That extern MUST be `extern "C"`: it is a ROM symbol spelled by hand, and in a
 * C++ TU a bare declaration mangles a second time to a name that exists nowhere.
 * build_pin.verify would not catch it -- a call is a relocation and match.compare
 * wildcards every relocated word. Only the link sees it.
 */
extern "C" int _ZN4cstd3absEi(int);

int ExpandingHeapAllocator::MaxAllocatableSize(int arg)
{
    int align = _ZN4cstd3absEi(arg);
    unsigned int bestSize = 0;
    unsigned int bestOffset = -1;
    char* n = *(char**)((char*)this + 0x24);
    while (n) {
        unsigned int base = (unsigned int)(n + 0x10);
        unsigned int aligned = ((align - 1) + base) & ~(align - 1);
        unsigned int end = *(int*)(n + 4) + base;
        if (aligned < end) {
            unsigned int usable = end - aligned;
            unsigned int offset = aligned - base;
            if (bestSize < usable || (bestSize == usable && bestOffset > offset)) {
                bestSize = usable;
                bestOffset = offset;
            }
        }
        n = *(char**)(n + 0xc);
    }
    return bestSize;
}
