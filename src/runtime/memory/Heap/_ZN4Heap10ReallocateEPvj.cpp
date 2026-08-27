//cpp
// @symbol _ZN4Heap10ReallocateEPvj
/* Heap::Reallocate(void*, u32) at 0x0203c578 -- forwards to slot 8 and hands
 * back the resulting SIZE, or 0 if the block could not be resized.
 *
 * RETURN TYPE: u32, and it took a caller to establish that it returns anything
 * at all. This was declared
 * `void' when the only evidence was its own body -- a pure forward, where the
 * return is unobservable because r0 just carries whatever the callee left.
 * SolidHeap::VResizeToFit branches on the result (`if (!Reallocate(...))'),
 * which a void declaration cannot compile at all. A forwarder's return type is
 * unobservable at its definition and observable at its callers; only one of
 * those was available before.
 *
 * It is a SIZE and not a moved pointer. Both allocators say so in their own
 * bodies: SolidHeapAllocator::Reallocate returns `size' or 0, and
 * ExpandingHeapAllocator::Reallocate returns `size', 0 or `node->size'. The
 * `void*' this once claimed came from a local shadow in
 * _ZN13ExpandingHeap11VReallocateEPvj.cpp that declared the allocator method
 * itself as pointer-returning -- a guess nothing ever checked, because a
 * forwarder cannot disagree with its callee about a value it merely passes on.
 *
 * Was a cast of `this' to a local `struct Base' with eight anonymous virtuals
 * padding `m' out to index 8. */
#include "Heap.h"

u32 Heap::Reallocate(void* ptr, u32 size)
{
    return VReallocate(ptr, size);
}
