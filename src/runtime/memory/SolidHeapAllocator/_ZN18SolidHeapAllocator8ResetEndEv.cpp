//cpp
// @symbol _ZN18SolidHeapAllocator8ResetEndEv
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::ResetEnd() at 0x0204ec10 -- uses `this`.
 *
 * Walks the saved-state chain and rewinds every entry's end pointer to the heap's
 * hard end (+0x1c), then rewinds the live free region's end too. LoadState can then
 * restore any of them without them still pointing into space that has been reclaimed.
 *
 * The free region is `this + 0x24`: p[0] begin, p[1] end, p[2] tail-of-state-chain,
 * and each state node's +0xc (node[3]) is the previous link. Those stay raw offsets
 * because the header's evidenced extent stops at 0x24.
 */
void SolidHeapAllocator::ResetEnd()
{
    int *node, *p = (int *)((char *)this + 0x24);
    for (node = (int *)p[2]; node; node = (int *)node[3])
        node[2] = *(int *)((char *)this + 0x1c);
    p[1] = *(int *)((char *)this + 0x1c);
}
