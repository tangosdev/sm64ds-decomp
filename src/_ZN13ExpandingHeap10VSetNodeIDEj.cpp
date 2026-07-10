//cpp
/* ExpandingHeap::VSetNodeID(unsigned) at 0x0203c3f8 -- Heap vtable slot.
 * Forwards to the allocator (ExpandingHeapAllocator* at this+0x14). */

typedef unsigned int u32;

class ExpandingHeapAllocator
{
public:
    void SetNodeID(u32 id);
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

    void VSetNodeID(u32 id);
};

void ExpandingHeap::VSetNodeID(u32 id)
{
    allocator->SetNodeID(id);
}
