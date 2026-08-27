//cpp
// @symbol _ZN13ExpandingHeap7VSizeofEPv
/* ExpandingHeap::VSizeof(void*) at 0x0203c444 -- Heap vtable slot 9. Node
 * headers carry their own size, so this ignores `this' entirely and forwards to
 * the STATIC ExpandingHeapAllocator::SizeofInternal -- which is why the ROM body
 * is `mov r0, r1' followed by a tail call: it moves the pointer argument into
 * r0 over the incoming `this'.
 *
 * SolidHeap's override crashes instead, because a linear allocator records no
 * sizes to look up. */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

u32 ExpandingHeap::VSizeof(void* ptr)
{
    return ExpandingHeapAllocator::SizeofInternal(ptr);
}
