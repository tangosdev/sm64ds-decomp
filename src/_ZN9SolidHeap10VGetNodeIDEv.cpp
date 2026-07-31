//cpp
#include "types.h"
// @symbol _ZN9SolidHeap10VGetNodeIDEv
/* recovered: named members + shared header, real C++ method */
#include "SolidHeap.h"
/* SolidHeap::VGetNodeID() at 0x0203c3d8 -- Heap vtable slot (VGetNodeID).
 * SolidHeap has no need for node IDs (linear allocator, no per-node tagging),
 * so this override simply returns 0. */
struct SolidHeap;

u32 SolidHeap::VGetNodeID()
{
    return 0;
}
