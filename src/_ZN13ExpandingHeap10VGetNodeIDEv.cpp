//cpp
/* ExpandingHeap::VGetNodeID() at 0x0203c3e0 -- Heap vtable slot (VGetNodeID).
 * Forwards to the allocator (ExpandingHeapAllocator* at this+0x14). */

typedef unsigned int u32;

class ExpandingHeapAllocator
{
public:
    u32 GetNodeID();
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

    u32 VGetNodeID();
};

u32 ExpandingHeap::VGetNodeID()
{
    return allocator->GetNodeID();
}
