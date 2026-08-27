//cpp
// @symbol _ZN22ExpandingHeapAllocator9GetNodeIDEv
#include "ExpandingHeapAllocator.h"

/* ExpandingHeapAllocator::GetNodeID() at 0x0204e0e0 -- a real method, uses `this`.
 *
 * Returns the ID stamped onto subsequently allocated nodes. The field is a u16 at
 * +0x34, which the header's padding does not yet name -- so `this` is re-viewed as
 * a u16 array and indexed at 26 (0x34 / 2), which is what the ROM does. Naming the
 * field is a header job and would let this read `return nodeID;`.
 */
u32 ExpandingHeapAllocator::GetNodeID()
{
    return ((u16 *)this)[26]; /* nodeID at +0x34 */
}
