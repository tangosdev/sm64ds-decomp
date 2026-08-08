//cpp
// @symbol _ZN4Heap8AllocateEji
/* Heap::Allocate(u32, int) at 0x0203c6cc -- the allocation entry point: call
 * slot 3, and if it comes back empty on a fail-fast heap, do not return the
 * NULL to the caller, Crash() instead.
 *
 * RETURN TYPE: void*, from the definition. This file used to declare `int
 * Heap::Allocate(...)' while _ZN4Heap8AllocateEj.cpp -- the sibling overload,
 * which forwards to this one -- declared the same function `void*'. Two files
 * disagreeing about one signature is exactly the debt the shadow structs
 * create. ExpandingHeap::VAllocate and SolidHeap::VAllocate both return void*,
 * so void* it is, and the two files now agree because they share a header.
 *
 * Was a cast of `this' to a local `struct Base' with three anonymous virtuals
 * padding `m' out to index 3, plus an address-cast read of `&unk_010'. */
#include "decl_common.h"
#include "Heap.h"

void* Heap::Allocate(u32 size, int align)
{
    void* block = VAllocate(size, align);
    if (block == 0 && (flags & 0x4000))
        Crash();
    return block;
}
