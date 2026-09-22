//cpp
// @symbol _ZN13ExpandingHeap10VSetNodeIDEj
/* ExpandingHeap::VSetNodeID(u32) at 0x0203c3f8 -- Heap vtable slot 13. Four
 * instructions: load the allocator from this+0x14 and tail-call it.
 *
 * RETURN TYPE: u32, changed from void. The slot returns the PREVIOUS id --
 * ExpandingHeapAllocator::SetNodeID is declared that way from its own body, and
 * SolidHeap's override materializes `mov r0,#0' in eight bytes, which void
 * cannot emit. This one is a pure tail call, so it byte-matches either way and
 * could never have settled the question itself; it just has to agree. */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

u32 ExpandingHeap::VSetNodeID(u32 id)
{
    return allocator->SetNodeID(id);
}
