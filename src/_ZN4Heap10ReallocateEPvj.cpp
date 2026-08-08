//cpp
// @symbol _ZN4Heap10ReallocateEPvj
/* Heap::Reallocate(void*, u32) at 0x0203c578 -- forwards to slot 8 and drops
 * the result. The slot returns the new block (ExpandingHeap::VReallocate is
 * declared void*), but this wrapper is void, so a caller that wants the
 * pointer has to go through the heap directly.
 *
 * Was a cast of `this' to a local `struct Base' with eight anonymous virtuals
 * padding `m' out to index 8. */
#include "Heap.h"

void Heap::Reallocate(void* ptr, u32 size)
{
    VReallocate(ptr, size);
}
