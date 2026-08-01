//cpp
#include "types.h"
// @symbol _ZN9SolidHeap10VSetNodeIDEj
/* recovered: named members + shared header, real C++ method */
#include "SolidHeap.h"
/* SolidHeap::VSetNodeID(u32 id) at 0x0203c3f0 -- Heap vtable slot (VSetNodeID).
 * SolidHeap has no need for node IDs (linear allocator), so this override
 * ignores the requested id and returns 0 (the previous node ID). */
struct SolidHeap;

u32 SolidHeap::VSetNodeID(unsigned int id_)
{
    u32 id = (u32)id_;

    return 0;
}
