//cpp
// @symbol _ZN9SolidHeap11VReallocateEPvj
/* SolidHeap::VReallocate(void*, u32) at 0x0203c558 -- Heap vtable slot 8.
 * Four instructions: load the allocator from this+0x14 and tail-call it.
 *
 * Returns the new SIZE, not a moved pointer -- SolidHeapAllocator::Reallocate
 * resizes in place and returns `size' or 0. This file used to declare its own
 * `class SolidHeap' with members named unk00..unk10 plus a `class
 * SolidHeapAllocator' whose Reallocate it declared pointer-returning, which is
 * where the tree's `void*' for this slot came from. */
#include "SolidHeap.h"
#include "SolidHeapAllocator.h"

u32 SolidHeap::VReallocate(void* ptr, u32 size)
{
    return allocator->Reallocate(ptr, size);
}
