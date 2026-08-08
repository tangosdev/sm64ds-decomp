//cpp
#include "types.h"
#include "ExpandingHeapAllocator.h"
/* ExpandingHeap::VDeallocateAll() at 0x0203c4b0 -- Heap vtable slot.
 * Forwards to the allocator (ExpandingHeapAllocator* at this+0x14),
 * passing the InvokeDeallocate trampoline as the visitor.
 *
 * THIS FILE CARRIED ITS OWN COPY OF ExpandingHeapAllocator and that copy is what kept
 * the wrong signature alive: it declared `DeallocateAll(Visitor* visitor, u32)`,
 * pointer-to-pointer-to-function, so the call mangled `PPFvPvPS_jE` and the linker was
 * satisfied. The tell was sitting in the call itself --
 *
 *     (Visitor*)&ExpandingHeapAllocator::InvokeDeallocate
 *
 * `&InvokeDeallocate` already IS a Visitor; casting it to `Visitor*` is a fudge the
 * wrong declaration demanded, and a cast that exists only to satisfy a signature is
 * evidence against the signature. With DeallocateAll taking a plain
 * DeallocationFunction, the trampoline goes in as itself and the cast disappears.
 * Byte-identical either way -- an address is an address -- which is exactly why nothing
 * caught this for as long as the declaration lived here instead of in the header.
 *
 * The local copy is gone; the shared header is the single declaration now. ExpandingHeap
 * keeps its local shape -- that class has not been reconstructed yet, and only the
 * allocator pointer at 0x14 is read here. */

class ExpandingHeap
{
public:
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u32 unk0c;
    u32 unk10;
    ExpandingHeapAllocator* allocator; /* 0x14 */

    void VDeallocateAll();
};

void ExpandingHeap::VDeallocateAll()
{
    allocator->DeallocateAll(&ExpandingHeapAllocator::InvokeDeallocate, 0);
}
