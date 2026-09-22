//cpp
// @symbol _ZN9SolidHeap10VGetNodeIDEv
/* SolidHeap::VGetNodeID() at 0x0203c3d8 -- Heap vtable slot 14. Eight bytes:
 * `mov r0,#0 / bx lr'. A linear allocator does not tag blocks, so there is no
 * node id to report. */
#include "SolidHeap.h"

u32 SolidHeap::VGetNodeID()
{
    return 0;
}
