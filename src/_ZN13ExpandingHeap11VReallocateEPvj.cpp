//cpp
// @symbol _ZN13ExpandingHeap11VReallocateEPvj
/* ExpandingHeap::VReallocate(void*, u32) at 0x0203c568 -- Heap vtable slot 8.
 * Four instructions: load the allocator from this+0x14 and tail-call it.
 *
 * RETURN TYPE: u32, changed from void*. THIS FILE IS WHERE THE void* CAME FROM.
 * It declared its own `class ExpandingHeapAllocator { void* Reallocate(void*,
 * u32); }', and that local guess propagated outwards until Heap vtable slot 8
 * was documented as returning a moved pointer. The real definition returns
 * `size', 0 or `node->size' -- a size every time. Nothing could catch it,
 * because a tail-call forwarder cannot disagree with its callee about a value it
 * merely passes on. */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

u32 ExpandingHeap::VReallocate(void* ptr, u32 size)
{
    return allocator->Reallocate(ptr, size);
}
