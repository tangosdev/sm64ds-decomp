//cpp
#include "types.h"
/* ExpandingHeap::VDeallocateAll() at 0x0203c4b0 -- Heap vtable slot.
 * Forwards to the allocator (ExpandingHeapAllocator* at this+0x14),
 * passing the InvokeDeallocate trampoline as the visitor. */
class ExpandingHeapAllocator
{
public:
    typedef void (*Visitor)(void*, ExpandingHeapAllocator*, u32);

    static void InvokeDeallocate(void* ptr, ExpandingHeapAllocator* allocator, u32 param);
    void DeallocateAll(Visitor* visitor, u32 param);
};

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
    allocator->DeallocateAll((ExpandingHeapAllocator::Visitor*)&ExpandingHeapAllocator::InvokeDeallocate, 0);
}
