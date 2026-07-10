//cpp
/* ExpandingHeap::VMemoryLeft() at 0x0203c5ac -- Heap vtable slot.
 * Forwards to the allocator (ExpandingHeapAllocator* at this+0x14). */

typedef unsigned int u32;

class ExpandingHeapAllocator
{
public:
    u32 MemoryLeft();
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

    u32 VMemoryLeft();
};

u32 ExpandingHeap::VMemoryLeft()
{
    return allocator->MemoryLeft();
}
