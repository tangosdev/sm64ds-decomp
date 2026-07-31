#include "types.h"
/* ExpandingHeapAllocator::GetNodeID() at 0x0204e0e0
 * Returns the current nodeID assigned to subsequently allocated nodes. nodeID is
 * a u16 field at offset 0x34 of ExpandingHeapAllocator (26 * sizeof(u16)); see
 * Memory.h. */
u32 _ZN22ExpandingHeapAllocator9GetNodeIDEv(u16 *self)
{
    return self[26]; /* nodeID at +0x34 */
}
