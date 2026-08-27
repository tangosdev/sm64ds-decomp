//cpp
// @symbol _ZN13ExpandingHeap14VDeallocateAllEv
/* ExpandingHeap::VDeallocateAll() at 0x0203c4b0 -- Heap vtable slot 5. Forwards
 * to the allocator, passing the InvokeDeallocate trampoline as the visitor.
 *
 * THIS FILE ONCE CARRIED ITS OWN COPY OF ExpandingHeapAllocator and that copy is
 * what kept a wrong signature alive: it declared `DeallocateAll(Visitor*
 * visitor, u32)', pointer-to-pointer-to-function, so the call mangled
 * `PPFvPvPS_jE' and the linker was satisfied. The tell was sitting in the call
 * itself --
 *
 *     (Visitor*)&ExpandingHeapAllocator::InvokeDeallocate
 *
 * `&InvokeDeallocate' already IS a Visitor; casting it to `Visitor*' is a fudge
 * the wrong declaration demanded, and a cast that exists only to satisfy a
 * signature is evidence against the signature. With DeallocateAll taking a plain
 * DeallocationFunction, the trampoline goes in as itself and the cast
 * disappears. Byte-identical either way -- an address is an address -- which is
 * exactly why nothing caught it for as long as the declaration lived here
 * instead of in the header.
 *
 * The last local shape goes with this slice: ExpandingHeap was still spelled
 * here as a flat `class ExpandingHeap { u32 unk00; ... }' because the class had
 * not been reconstructed. It has now, so the allocator comes from the header
 * like everything else. */
#include "ExpandingHeap.h"
#include "ExpandingHeapAllocator.h"

void ExpandingHeap::VDeallocateAll()
{
    allocator->DeallocateAll(&ExpandingHeapAllocator::InvokeDeallocate, 0);
}
