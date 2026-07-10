//cpp
/* ExpandingHeap::VAllocate(unsigned, int) at 0x0203c6bc -- Heap vtable slot.
 * Forwards to the allocator (ExpandingHeapAllocator* at this+0x14). */

typedef unsigned int u32;

class ExpandingHeapAllocator
{
public:
    void* Allocate(u32 size, int align);
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

    void* VAllocate(u32 size, int align);
};

void* ExpandingHeap::VAllocate(u32 size, int align)
{
    return allocator->Allocate(size, align);
}
